#include "AdminConsole.h"

#include <QCoreApplication>
#include <QMetaObject>
#include <QObject>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <future>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

/** Estado compartilhado entre a GUI e a thread do console. */
struct EstadoAdmin {
    std::atomic<bool> vivo{true}; // false depois que o AdminConsole e destruido
    RepositorioGenerico<Filme>* filmes;
    RepositorioGenerico<Sessao>* sessoes;
    RepositorioGenerico<ItemBomboniere>* bomboniere;
    AdminConsole::Ganchos ganchos;
};

namespace {

// Limites e valores padrao ao cadastrar uma sala nova.
constexpr int kFileirasPadrao = 6;
constexpr int kColunasPadrao  = 8;
constexpr int kMaxFileiras    = Sala::kMaxFileiras;
constexpr int kMaxColunas     = 20;

// ---------- Leitura de console (so roda na thread do admin) ----------

// Le uma linha inteira; retorna false se o stdin fechou (EOF).
bool lerLinha(const std::string& prompt, std::string& out) {
    std::cout << prompt << std::flush;
    return static_cast<bool>(std::getline(std::cin, out));
}

// Repete ate receber um inteiro em [minimo, maximo]. Linha vazia usa "padrao", se houver.
// Retorna false se o stdin fechou.
bool lerInteiro(const std::string& prompt, int minimo, int maximo, int& out,
                std::optional<int> padrao = std::nullopt) {
    std::string linha;
    while (lerLinha(prompt, linha)) {
        if (linha.empty() && padrao) {
            out = *padrao;
            return true;
        }
        try {
            size_t pos = 0;
            int valor = std::stoi(linha, &pos);
            if (pos == linha.size() && valor >= minimo && valor <= maximo) {
                out = valor;
                return true;
            }
        } catch (...) {}
        std::cout << "[ADMIN] Valor invalido. Informe um inteiro entre " << minimo << " e " << maximo << ".\n";
    }
    return false;
}

// Aceita "12.5" ou "12,5" com valor >= minimo. Retorna false se o stdin fechou.
bool lerDecimal(const std::string& prompt, double minimo, double& out) {
    std::string linha;
    while (lerLinha(prompt, linha)) {
        for (char& c : linha) if (c == ',') c = '.';
        try {
            size_t pos = 0;
            double valor = std::stod(linha, &pos);
            if (pos == linha.size() && valor >= minimo) {
                out = valor;
                return true;
            }
        } catch (...) {}
        std::cout << "[ADMIN] Valor invalido. Informe um numero >= " << minimo << ".\n";
    }
    return false;
}

// Formato "HH:MM" (00:00 a 23:59).
bool horarioValido(const std::string& h) {
    if (h.size() != 5 || h[2] != ':') return false;
    for (size_t i : {0u, 1u, 3u, 4u}) {
        if (!std::isdigit(static_cast<unsigned char>(h[i]))) return false;
    }
    const int hh = std::stoi(h.substr(0, 2));
    const int mm = std::stoi(h.substr(3, 2));
    return hh <= 23 && mm <= 59;
}

// ---------- Cadastro de sessao ----------

struct SalaInfo {
    int numero;
    std::string tipo;
    int fileiras;
    int colunas;
};

// Copia dos dados dos repositorios, tirada na thread da GUI, para a thread do admin
// poder listar/validar sem tocar nos repositorios (evita data race).
struct SnapshotAdmin {
    std::vector<std::pair<int, std::string>> filmes;     // (id, titulo)
    std::map<int, SalaInfo> salas;                        // salas ja cadastradas, por numero
    std::set<std::pair<int, std::string>> ocupacao;       // (numero da sala, horario)
};

struct NovaSessao {
    int idFilme = 0;
    std::string horario;
    TipoAudio audio = TipoAudio::DUBLADO;
    TecnologiaExibicao tecnologia = TecnologiaExibicao::DOIS_D;
    int numeroSala = 0;
    std::string tipoSala;
    int fileiras = kFileirasPadrao;
    int colunas = kColunasPadrao;
    double preco = 0.0;
};

enum class ResultadoDialogo { CONCLUIDO, CANCELADO, STDIN_FECHADO };

// Conduz as perguntas do cadastro de sessao. Nao mexe em nenhum repositorio.
ResultadoDialogo coletarNovaSessao(const SnapshotAdmin& snap, NovaSessao& out) {
    const int kSemLimite = std::numeric_limits<int>::max();

    std::cout << "\n--- FILMES DISPONIVEIS ---\n";
    for (const auto& f : snap.filmes) {
        std::cout << " [ID " << f.first << "] " << f.second << "\n";
    }

    // Filme
    while (true) {
        int id = 0;
        if (!lerInteiro("ID do filme (0 = cancelar): ", 0, kSemLimite, id)) return ResultadoDialogo::STDIN_FECHADO;
        if (id == 0) return ResultadoDialogo::CANCELADO;
        const bool existe = std::any_of(snap.filmes.begin(), snap.filmes.end(),
                                        [id](const auto& f) { return f.first == id; });
        if (existe) { out.idFilme = id; break; }
        std::cout << "[ADMIN] Nao existe filme com ID " << id << ".\n";
    }

    // Horario
    std::string horario;
    while (true) {
        if (!lerLinha("Horario (HH:MM): ", horario)) return ResultadoDialogo::STDIN_FECHADO;
        if (horarioValido(horario)) break;
        std::cout << "[ADMIN] Horario invalido. Use o formato HH:MM, por exemplo 18:30.\n";
    }
    out.horario = horario;

    // Audio e tecnologia
    int escolha = 0;
    if (!lerInteiro("Audio (1 = Dublado, 2 = Legendado): ", 1, 2, escolha)) return ResultadoDialogo::STDIN_FECHADO;
    out.audio = (escolha == 1) ? TipoAudio::DUBLADO : TipoAudio::LEGENDADO;
    if (!lerInteiro("Tecnologia (1 = 2D, 2 = 3D): ", 1, 2, escolha)) return ResultadoDialogo::STDIN_FECHADO;
    out.tecnologia = (escolha == 1) ? TecnologiaExibicao::DOIS_D : TecnologiaExibicao::TRES_D;

    // Sala: se o numero ja existe, reaproveita tipo e dimensoes
    if (!lerInteiro("Numero da sala (1-99): ", 1, 99, out.numeroSala)) return ResultadoDialogo::STDIN_FECHADO;
    auto it = snap.salas.find(out.numeroSala);
    if (it != snap.salas.end()) {
        out.tipoSala = it->second.tipo;
        out.fileiras = it->second.fileiras;
        out.colunas  = it->second.colunas;
        std::cout << "[ADMIN] Sala " << out.numeroSala << " ja existe (" << out.fileiras << "x" << out.colunas
                  << "). Reutilizando tipo e dimensoes.\n";
    } else {
        if (!lerInteiro("Tipo da sala (1 = Padrao, 2 = 3D, 3 = VIP, Enter = 1): ", 1, 3, escolha, 1)) return ResultadoDialogo::STDIN_FECHADO;
        out.tipoSala = (escolha == 1) ? "Padrão" : (escolha == 2) ? "3D" : "VIP";
        if (!lerInteiro("Fileiras (1-26, Enter = 6): ", 1, kMaxFileiras, out.fileiras, kFileirasPadrao)) return ResultadoDialogo::STDIN_FECHADO;
        if (!lerInteiro("Colunas (1-20, Enter = 8): ", 1, kMaxColunas, out.colunas, kColunasPadrao)) return ResultadoDialogo::STDIN_FECHADO;
    }

    if (snap.ocupacao.count({out.numeroSala, out.horario}) > 0) {
        std::cout << "[ADMIN] A sala " << out.numeroSala << " ja tem sessao as " << out.horario << ".\n";
        return ResultadoDialogo::CANCELADO;
    }

    if (!lerDecimal("Preco base do ingresso (R$): ", 0.01, out.preco)) return ResultadoDialogo::STDIN_FECHADO;
    return ResultadoDialogo::CONCLUIDO;
}


std::atomic<bool> g_encerrando{false}; // true quando o QApplication esta terminando

// Agenda uma funcao para rodar na thread da GUI. Retorna false se o app ja esta encerrando.
bool postarNaGui(std::function<void()> fn) {
    if (g_encerrando) return false;
    QCoreApplication* app = QCoreApplication::instance();
    if (!app) return false;
    QMetaObject::invokeMethod(app, std::move(fn), Qt::QueuedConnection);
    return true;
}

// Copia os dados dos repositorios (feita na thread da GUI) e espera por ela sem
// nunca bloquear para sempre: se o app encerrar, desiste.
bool tirarSnapshot(const std::shared_ptr<EstadoAdmin>& est, SnapshotAdmin& out) {
    auto prom = std::make_shared<std::promise<SnapshotAdmin>>();
    std::future<SnapshotAdmin> fut = prom->get_future();

    const bool agendado = postarNaGui([est, prom]() {
        SnapshotAdmin snap;
        if (est->vivo) {
            for (size_t i = 0; i < est->filmes->tamanho(); ++i) {
                const Filme& f = est->filmes->obter(i);
                snap.filmes.emplace_back(f.getId(), f.getTitulo());
            }
            for (size_t i = 0; i < est->sessoes->tamanho(); ++i) {
                const Sessao& s = est->sessoes->obter(i);
                const Sala& sala = s.getSala();
                snap.salas[sala.getNumero()] = SalaInfo{sala.getNumero(), sala.getTipoSala(),
                                                       sala.getQuantidadeFileiras(), sala.getQuantidadeColunas()};
                snap.ocupacao.insert({sala.getNumero(), s.getHorario()});
            }
        }
        prom->set_value(std::move(snap));
    });
    if (!agendado) return false;

    while (fut.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) {
        if (g_encerrando || !est->vivo) return false;
    }
    try {
        out = fut.get();
    } catch (const std::future_error&) {
        return false; // a tarefa foi descartada sem executar
    }
    return true;
}

void executarConsole(std::shared_ptr<EstadoAdmin> est) {
    std::cout << "========================================\n";
    std::cout << " [MODO ADMIN CONSOLE ATIVO]\n";
    std::cout << " Comandos disponiveis:\n";
    std::cout << "   1 - Adicionar Filme\n";
    std::cout << "   2 - Adicionar Produto Bomboniere\n";
    std::cout << "   3 - Listar Tudo (Filmes, Sessoes e Bomboniere)\n";
    std::cout << "   4 - Adicionar Sessao\n";
    std::cout << "========================================\n";

    std::string opcao;
    while (lerLinha("> ", opcao)) {
        if (!est->vivo || g_encerrando) break;

        if (opcao == "1") {
            std::string titulo, genero;
            int duracao = 0, classificacao = 0;

            if (!lerLinha("Titulo do filme: ", titulo)) break;
            if (titulo.empty()) { std::cout << "[ADMIN] Titulo vazio. Cancelado.\n"; continue; }
            if (!lerInteiro("Duracao (min): ", 1, 1000, duracao)) break;
            if (!lerInteiro("Classificacao (anos, 0 = livre): ", 0, 18, classificacao)) break;
            if (!lerLinha("Genero: ", genero)) break;

            // O Filme usa um contador estatico de id: criar na thread da GUI.
            postarNaGui([est, titulo, duracao, classificacao, genero]() {
                if (!est->vivo) return;
                Filme novo(titulo, duracao, classificacao, genero);
                const int idNovo = novo.getId();
                est->filmes->adicionar(novo);
                if (est->ganchos.aoMudarCatalogo) est->ganchos.aoMudarCatalogo();
                std::cout << "[ADMIN] Filme adicionado com sucesso! (ID " << idNovo << ")\n" << std::flush;
            });
        }
        else if (opcao == "2") {
            std::string nome;
            double preco = 0.0;

            if (!lerLinha("Nome do produto: ", nome)) break;
            if (nome.empty()) { std::cout << "[ADMIN] Nome vazio. Cancelado.\n"; continue; }
            if (!lerDecimal("Preco (R$): ", 0.0, preco)) break;

            postarNaGui([est, nome, preco]() {
                if (!est->vivo) return;
                // O ItemBomboniere usa um contador estatico de id: criar na thread da GUI.
                est->bomboniere->adicionar(ItemBomboniere(nome, preco, ""));
                if (est->ganchos.aoMudarBomboniere) est->ganchos.aoMudarBomboniere();
                std::cout << "[ADMIN] Produto de bomboniere adicionado com sucesso!\n" << std::flush;
            });
        }
        else if (opcao == "3") {
            postarNaGui([est]() {
                if (!est->vivo) return;
                std::cout << "\n================ [RELATORIO DO SISTEMA] ================\n";

                std::cout << "\n--- FILMES CADASTRADOS ---\n";
                for (size_t i = 0; i < est->filmes->tamanho(); ++i) {
                    const Filme& f = est->filmes->obter(i);
                    std::cout << " * [ID " << f.getId() << "] " << f.getTitulo() << " (" << f.getGenero() << ", " << f.getDuracao() << " min)\n";
                }

                std::cout << "\n--- SESSOES DISPONIVEIS ---\n";
                for (size_t i = 0; i < est->sessoes->tamanho(); ++i) {
                    const Sessao& s = est->sessoes->obter(i);
                    std::cout << " * [Sessao ID " << s.getId() << "] Filme: " << s.getFilme().getTitulo()
                              << " | Horario: " << s.getHorario() << " | Preco Base: R$ " << s.getPrecoBase() << "\n";
                }

                std::cout << "\n--- PRODUTOS DA BOMBONIERE ---\n";
                for (size_t i = 0; i < est->bomboniere->tamanho(); ++i) {
                    const ItemBomboniere& item = est->bomboniere->obter(i);
                    std::cout << " * [ID " << item.getId() << "] " << item.getNome() << " - R$ " << item.getPreco() << "\n";
                }
                std::cout << "=========================================================\n\n" << std::flush;
            });
        }
        else if (opcao == "4") {
            SnapshotAdmin snap;
            if (!tirarSnapshot(est, snap)) break;
            if (snap.filmes.empty()) {
                std::cout << "[ADMIN] Nenhum filme cadastrado. Cadastre um filme antes (opcao 1).\n";
                continue;
            }

            NovaSessao nova;
            const ResultadoDialogo r = coletarNovaSessao(snap, nova);
            if (r == ResultadoDialogo::STDIN_FECHADO) break;
            if (r == ResultadoDialogo::CANCELADO) { std::cout << "[ADMIN] Cadastro de sessao cancelado.\n"; continue; }

            postarNaGui([est, nova]() {
                if (!est->vivo) return;

                const Filme* filme = nullptr;
                int novoId = 1;
                for (size_t i = 0; i < est->filmes->tamanho(); ++i) {
                    if (est->filmes->obter(i).getId() == nova.idFilme) { filme = &est->filmes->obter(i); break; }
                }
                for (size_t i = 0; i < est->sessoes->tamanho(); ++i) {
                    novoId = std::max(novoId, est->sessoes->obter(i).getId() + 1);
                }
                if (!filme) {
                    std::cout << "[ADMIN] Filme nao encontrado. Sessao nao cadastrada.\n" << std::flush;
                    return;
                }

                try {
                    // Sessao guarda copias de Filme e Sala: cada sessao tem a sua propria lotacao.
                    est->sessoes->adicionar(Sessao(novoId, *filme,
                        Sala(nova.numeroSala, nova.fileiras, nova.colunas, nova.tipoSala),
                        nova.horario, nova.audio, nova.tecnologia, nova.preco));
                } catch (const std::exception& e) {
                    std::cout << "[ADMIN] Nao foi possivel criar a sessao: " << e.what() << "\n" << std::flush;
                    return;
                }
                if (est->ganchos.aoMudarCatalogo) est->ganchos.aoMudarCatalogo();
                std::cout << "[ADMIN] Sessao ID " << novoId << " adicionada para \"" << filme->getTitulo() << "\".\n" << std::flush;
            });
        }
        else if (!opcao.empty()) {
            std::cout << "[ADMIN] Comando desconhecido: " << opcao << "\n";
        }
    }
}

} // namespace

AdminConsole::AdminConsole(RepositorioGenerico<Filme>& filmes,
                           RepositorioGenerico<Sessao>& sessoes,
                           RepositorioGenerico<ItemBomboniere>& bomboniere,
                           Ganchos ganchos)
    : estado(std::make_shared<EstadoAdmin>()) {
    estado->filmes = &filmes;
    estado->sessoes = &sessoes;
    estado->bomboniere = &bomboniere;
    estado->ganchos = std::move(ganchos);
}

AdminConsole::~AdminConsole() {
    // Roda na thread da GUI, a mesma que executa as tarefas postadas: sem corrida.
    estado->vivo = false;
}

void AdminConsole::iniciar() {
    QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                     []() { g_encerrando = true; });
    // detach: a thread pode ficar bloqueada em std::cin ate o processo terminar.
    std::thread(executarConsole, estado).detach();
}
