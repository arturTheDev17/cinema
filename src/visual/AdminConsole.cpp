/**
 * @file AdminConsole.cpp
 * @brief Implementação do console administrativo assíncrono (cadastro, edição e remoção em paralelo à GUI).
 */

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

/**
 * @struct EstadoAdmin
 * @brief Estado compartilhado de forma segura entre a GUI e a thread do console.
 */
struct EstadoAdmin {
    std::atomic<bool> vivo{true};                    ///< false depois que o AdminConsole é destruído.
    RepositorioGenerico<Filme>* filmes;              ///< Ponteiro para o repositório de filmes.
    RepositorioGenerico<Sessao>* sessoes;            ///< Ponteiro para o repositório de sessões.
    RepositorioGenerico<ItemBomboniere>* bomboniere; ///< Ponteiro para o repositório da bomboniere.
    AdminConsole::Ganchos ganchos;                   ///< Callbacks executados na thread da GUI.
};

namespace {

// Limites e valores padrão ao cadastrar uma sala nova.
constexpr int kFileirasPadrao = 6;
constexpr int kColunasPadrao  = 8;
constexpr int kMaxFileiras    = Sala::kMaxFileiras;
constexpr int kMaxColunas     = 20;

// ---------- Leitura de console (só roda na thread do admin) ----------

/**
 * @brief Lê uma linha inteira da entrada padrão; retorna false se o stdin fechou (EOF).
 * @param[in] prompt Mensagem exibida antes da leitura.
 * @param[out] out Linha lida do teclado.
 * @return true se leu com sucesso; false se o fluxo de entrada foi encerrado.
 */
bool lerLinha(const std::string& prompt, std::string& out) {
    std::cout << prompt << std::flush;
    return static_cast<bool>(std::getline(std::cin, out));
}

/**
 * @brief Repete a leitura até obter um inteiro em [minimo, maximo]; linha vazia usa o valor padrão, se houver.
 * @param[in] prompt Mensagem exibida ao operador.
 * @param[in] minimo Limite inferior aceito.
 * @param[in] maximo Limite superior aceito.
 * @param[out] out Inteiro validado.
 * @param[in] padrao Valor opcional adotado ao pressionar Enter sem digitar texto.
 * @return true se um inteiro válido foi obtido; false se o stdin fechou.
 */
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

/**
 * @brief Lê um decimal >= minimo aceitando "." ou "," como separador.
 * @param[in] prompt Mensagem exibida ao operador.
 * @param[in] minimo Valor mínimo permitido.
 * @param[out] out Valor decimal validado.
 * @return true se válido; false se o stdin fechou.
 */
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

/**
 * @brief Valida o formato "HH:MM" entre 00:00 e 23:59.
 * @param[in] h String de horário.
 * @return true se o horário for sintaticamente e numericamente válido.
 */
bool horarioValido(const std::string& h) {
    if (h.size() != 5 || h[2] != ':') return false;
    for (size_t i : {0u, 1u, 3u, 4u}) {
        if (!std::isdigit(static_cast<unsigned char>(h[i]))) return false;
    }
    const int hh = std::stoi(h.substr(0, 2));
    const int mm = std::stoi(h.substr(3, 2));
    return hh <= 23 && mm <= 59;
}

// ---------- Cadastro de sessão ----------

/**
 * @struct SalaInfo
 * @brief Metadados resumidos de uma sala já existente no sistema.
 */
struct SalaInfo {
    int numero;       ///< Número identificador da sala.
    std::string tipo; ///< Tipo da sala.
    int fileiras;     ///< Quantidade de fileiras.
    int colunas;      ///< Quantidade de colunas.
};

/**
 * @struct SnapshotAdmin
 * @brief Cópia dos dados dos repositórios tirada na thread da GUI para leitura segura pela thread do admin.
 */
struct SnapshotAdmin {
    std::vector<std::pair<int, std::string>> filmes; ///< Pares (id, título) dos filmes disponíveis.
    std::map<int, SalaInfo> salas;                   ///< Salas já cadastradas, indexadas por número.
    std::set<std::pair<int, std::string>> ocupacao;  ///< Pares (número da sala, horário) já ocupados.
};

/**
 * @struct NovaSessao
 * @brief Dados coletados via console para criação de uma nova sessão.
 */
struct NovaSessao {
    int idFilme = 0;                                           ///< ID do filme escolhido.
    std::string horario;                                       ///< Horário no formato "HH:MM".
    TipoAudio audio = TipoAudio::DUBLADO;                      ///< Áudio escolhido.
    TecnologiaExibicao tecnologia = TecnologiaExibicao::DOIS_D;///< Tecnologia de projeção.
    int numeroSala = 0;                                        ///< Número da sala.
    std::string tipoSala;                                      ///< Categoria da sala.
    int fileiras = kFileirasPadrao;                            ///< Fileiras da sala.
    int colunas = kColunasPadrao;                              ///< Colunas da sala.
    double preco = 0.0;                                        ///< Preço base do ingresso.
};

/**
 * @enum ResultadoDialogo
 * @brief Estados finais do diálogo de coleta de dados no terminal.
 */
enum class ResultadoDialogo {
    CONCLUIDO,    ///< Coleta finalizada com dados válidos.
    CANCELADO,    ///< Operação cancelada pelo usuário ou por conflito de horário.
    STDIN_FECHADO ///< Entrada padrão encerrada.
};

/**
 * @brief Conduz as perguntas do cadastro de sessão sem tocar diretamente em nenhum repositório.
 * @param[in] snap Fotografia consistente dos repositórios.
 * @param[out] out Estrutura preenchida com os dados da nova sessão.
 * @return ResultadoDialogo indicando conclusão, cancelamento ou EOF.
 */
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

    // Horário
    std::string horario;
    while (true) {
        if (!lerLinha("Horario (HH:MM): ", horario)) return ResultadoDialogo::STDIN_FECHADO;
        if (horarioValido(horario)) break;
        std::cout << "[ADMIN] Horario invalido. Use o formato HH:MM, por exemplo 18:30.\n";
    }
    out.horario = horario;

    // Áudio e tecnologia
    int escolha = 0;
    if (!lerInteiro("Audio (1 = Dublado, 2 = Legendado): ", 1, 2, escolha)) return ResultadoDialogo::STDIN_FECHADO;
    out.audio = (escolha == 1) ? TipoAudio::DUBLADO : TipoAudio::LEGENDADO;
    if (!lerInteiro("Tecnologia (1 = 2D, 2 = 3D): ", 1, 2, escolha)) return ResultadoDialogo::STDIN_FECHADO;
    out.tecnologia = (escolha == 1) ? TecnologiaExibicao::DOIS_D : TecnologiaExibicao::TRES_D;

    // Sala: se o número já existe, reaproveita tipo e dimensões
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


std::atomic<bool> g_encerrando{false}; ///< true quando o QApplication está terminando.

/**
 * @brief Agenda uma função para rodar na thread da GUI via fila de eventos do Qt.
 * @param[in] fn Função a ser executada na thread principal.
 * @return false se a aplicação já estiver encerrando; true caso contrário.
 */
bool postarNaGui(std::function<void()> fn) {
    if (g_encerrando) return false;
    QCoreApplication* app = QCoreApplication::instance();
    if (!app) return false;
    QMetaObject::invokeMethod(app, std::move(fn), Qt::QueuedConnection);
    return true;
}

/**
 * @brief Solicita uma cópia dos dados dos repositórios na thread da GUI e aguarda com timeout.
 * @param[in] est Ponteiro compartilhado para o estado administrativo.
 * @param[out] out Snapshot preenchido.
 * @return true se concluído; false se o aplicativo encerrou antes da resposta.
 */
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
        return false; // A tarefa foi descartada sem executar
    }
    return true;
}

/**
 * @brief Loop interativo do console administrativo com comandos de cadastro, listagem, edição e remoção.
 * @param[in] est Ponteiro compartilhado para EstadoAdmin.
 */
void executarConsole(std::shared_ptr<EstadoAdmin> est) {
    std::cout << "========================================\n";
    std::cout << " [MODO ADMIN CONSOLE ATIVO]\n";
    std::cout << " Comandos disponiveis:\n";
    std::cout << "   1 - Adicionar Filme\n";
    std::cout << "   2 - Adicionar Produto Bomboniere\n";
    std::cout << "   3 - Listar Tudo (Filmes, Sessoes e Bomboniere)\n";
    std::cout << "   4 - Adicionar Sessao\n";
    std::cout << "   5 - Atualizar preco de Produto\n";
    std::cout << "   6 - Remover Produto\n";
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

            // O Filme usa um contador estático de id: criar na thread da GUI.
            postarNaGui([est, titulo, duracao, classificacao, genero]() {
                if (!est->vivo) return;
                try {
                    Filme novo(titulo, duracao, classificacao, genero); // Valida os dados
                    const int idNovo = novo.getId();
                    est->filmes->adicionar(novo);
                    if (est->ganchos.aoMudarCatalogo) est->ganchos.aoMudarCatalogo();
                    std::cout << "[ADMIN] Filme adicionado com sucesso! (ID " << idNovo << ")\n" << std::flush;
                } catch (const std::exception& e) {
                    std::cout << "[ADMIN] Filme nao cadastrado: " << e.what() << "\n" << std::flush;
                }
            });
        }
        else if (opcao == "2") {
            std::string nome;
            double preco = 0.0;

            if (!lerLinha("Nome do produto: ", nome)) break;
            if (nome.empty()) { std::cout << "[ADMIN] Nome vazio. Cancelado.\n"; continue; }
            if (!lerDecimal("Preco (R$): ", 0.01, preco)) break;

            postarNaGui([est, nome, preco]() {
                if (!est->vivo) return;
                // O ItemBomboniere usa um contador estático de id: criar na thread da GUI.
                try {
                    est->bomboniere->adicionar(ItemBomboniere(nome, preco, "")); // Valida nome e preço
                    if (est->ganchos.aoMudarBomboniere) est->ganchos.aoMudarBomboniere();
                    std::cout << "[ADMIN] Produto de bomboniere adicionado com sucesso!\n" << std::flush;
                } catch (const std::exception& e) {
                    std::cout << "[ADMIN] Produto nao cadastrado: " << e.what() << "\n" << std::flush;
                }
            });
        }
        else if (opcao == "3") {
            postarNaGui([est]() {
                if (!est->vivo) return;
                std::cout << "\n================ [RELATORIO DO SISTEMA] ================\n";

                std::cout << "\n--- FILMES CADASTRADOS ---\n";
                for (const Filme& f : est->filmes->listarTodos()) std::cout << " * " << f << "\n";

                std::cout << "\n--- SESSOES DISPONIVEIS (por horario) ---\n";
                std::vector<const Sessao*> ordenadas;
                for (const Sessao& s : est->sessoes->listarTodos()) ordenadas.push_back(&s);
                std::sort(ordenadas.begin(), ordenadas.end(), [](const Sessao* a, const Sessao* b) { return *a < *b; });
                for (const Sessao* s : ordenadas) std::cout << " * " << *s << "\n";

                std::cout << "\n--- PRODUTOS DA BOMBONIERE ---\n";
                for (const ItemBomboniere& item : est->bomboniere->listarTodos()) std::cout << " * " << item << "\n";
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

                const Filme* filme = est->filmes->buscar(nova.idFilme);
                int novoId = 1;
                for (size_t i = 0; i < est->sessoes->tamanho(); ++i) {
                    novoId = std::max(novoId, est->sessoes->obter(i).getId() + 1);
                }
                if (!filme) {
                    std::cout << "[ADMIN] Filme nao encontrado. Sessao nao cadastrada.\n" << std::flush;
                    return;
                }

                try {
                    // Sessao guarda cópias de Filme e Sala: cada sessão tem a sua própria lotação.
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
        else if (opcao == "5") {
            constexpr int kMaxId = std::numeric_limits<int>::max();
            int id = 0;
            double novoPreco = 0.0;

            if (!lerInteiro("ID do produto (0 = cancelar): ", 0, kMaxId, id)) break;
            if (id == 0) { std::cout << "[ADMIN] Atualizacao cancelada.\n"; continue; }
            if (!lerDecimal("Novo preco (R$): ", 0.01, novoPreco)) break;

            postarNaGui([est, id, novoPreco]() {
                if (!est->vivo) return;
                ItemBomboniere* item = est->bomboniere->buscar(id);
                if (!item) {
                    std::cout << "[ADMIN] Produto ID " << id << " nao encontrado.\n" << std::flush;
                    return;
                }
                const double antigo = item->getPreco();
                try {
                    item->setPreco(novoPreco); // Só o preço muda; nome, id e imagem ficam intactos
                } catch (const std::exception& e) {
                    std::cout << "[ADMIN] " << e.what() << "\n" << std::flush;
                    return;
                }
                if (est->ganchos.aoMudarBomboniere) est->ganchos.aoMudarBomboniere();
                std::cout << "[ADMIN] Preco de \"" << item->getNome() << "\" alterado de R$ " << antigo
                          << " para R$ " << novoPreco << ".\n" << std::flush;
            });
        }
        else if (opcao == "6") {
            constexpr int kMaxId = std::numeric_limits<int>::max();
            int id = 0;
            std::string resposta;

            if (!lerInteiro("ID do produto (0 = cancelar): ", 0, kMaxId, id)) break;
            if (id == 0) { std::cout << "[ADMIN] Remocao cancelada.\n"; continue; }
            if (!lerLinha("Remover o produto ID " + std::to_string(id) + "? (s/n): ", resposta)) break;
            if (resposta != "s" && resposta != "S") { std::cout << "[ADMIN] Remocao cancelada.\n"; continue; }

            postarNaGui([est, id]() {
                if (!est->vivo) return;
                const ItemBomboniere* item = est->bomboniere->buscar(id);
                if (!item) {
                    std::cout << "[ADMIN] Produto ID " << id << " nao encontrado.\n" << std::flush;
                    return;
                }
                const std::string nome = item->getNome(); // Copia antes de remover
                // Seguro: o Carrinho guarda cópias dos itens, nunca ponteiros para o repositório.
                est->bomboniere->removerPorId(id);
                if (est->ganchos.aoMudarBomboniere) est->ganchos.aoMudarBomboniere();
                std::cout << "[ADMIN] Produto \"" << nome << "\" removido.\n" << std::flush;
            });
        }
        else if (!opcao.empty()) {
            std::cout << "[ADMIN] Comando desconhecido: " << opcao << "\n";
        }
    }
}

} // namespace

/** @details Inicializa o estado compartilhado com ponteiros para os repositórios e os ganchos visuais. */
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

/** @details Marca estado->vivo como false na thread da GUI evitando condições de corrida no fechamento. */
AdminConsole::~AdminConsole() {
    // Roda na thread da GUI, a mesma que executa as tarefas postadas: sem corrida.
    estado->vivo = false;
}

/** @details Conecta QCoreApplication::aboutToQuit para sinalizar encerramento e inicia a thread destacada. */
void AdminConsole::iniciar() {
    QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                     []() { g_encerrando = true; });
    // detach: a thread pode ficar bloqueada em std::cin até o processo terminar.
    std::thread(executarConsole, estado).detach();
}