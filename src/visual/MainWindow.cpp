#include "MainWindow.h"
#include "TelaInicialWidget.h"
#include "Tela1SelecaoFilmeWidget.h"
#include "Tela2AssentosWidget.h"
#include "Tela3BomboniereWidget.h"
#include "Tela4PagamentoWidget.h"

#include <QIcon>
#include <QKeySequence>
#include <QMetaObject>
#include <QPointer>
#include <QPushButton>
#include <QShortcut>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#endif

void MainWindow::aplicarEstiloGlobal() {
    QString estiloGlobal = R"(
        QWidget {
            background-color: #181c25;
            color: #fbfef9;
            font-family: 'Segoe UI', Arial, sans-serif;
        }

        QGroupBox {
            border: 1px solid #3d4d52;
            border-radius: 3px;
            font-weight: 700;
            font-size: 20px;
            margin-top: 16px; 
            padding-top: 10px; 
        }
        
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            left: 12px;
            padding: 0 6px; 
            color: #dec402;
            font-size: 20px;
        }

        QScrollArea, QListWidget, QScrollArea > QWidget > QWidget {
          border: 0px solid #dec402;
        }

        QGroupBox QLabel {
            background-color: transparent;
            color: #fbfef9;
        }

        QListWidget {
            background-color: #181c25;
            color: #fbfef9;
            selection-background-color: #D32222;
            selection-color: #fbfef9;
        }

        QSpinBox {
            background-color: #181c25;
            color: #fbfef9;
        }
        QSpinBox::up-button, QSpinBox::down-button {
            background-color: #3d4d52;
        }

        QPushButton {
            background-color: #3d4d52;
            color: #fbfef9;
            font-weight: bold;
            border-radius: 3px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #dec402;
            color: #181c25;
        }
        QPushButton:disabled {
            background-color: #2b353b;
            color: #777777;
        }

        QScrollBar:vertical {
            border: none;
            background: #181c25;
            width: 8px;
            margin: 0px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background: #3d4d52;
            min-height: 20px;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical:hover {
            background: #dec402;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: none;
        }

        QScrollBar:horizontal {
            border: none;
            background: #181c25;
            height: 8px;
            margin: 0px;
            border-radius: 4px;
        }
        QScrollBar::handle:horizontal {
            background: #3d4d52;
            min-width: 20px;
            border-radius: 4px;
        }
        QScrollBar::handle:horizontal:hover {
            background: #dec402;
        }
        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }
        QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
            background: none;
        }
    )";

    this->setStyleSheet(estiloGlobal);
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowIcon(QIcon(":/logo.png"));
    aplicarEstiloGlobal();
    setWindowTitle("CinemaMecha");
    resize(1100, 720);
        

#ifdef _WIN32
    HWND hwnd = reinterpret_cast<HWND>(this->winId());
    BOOL useDarkMode = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
#endif

    inicializarDados();

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    telaInicial = new TelaInicialWidget(this);
    tela1 = new Tela1SelecaoFilmeWidget(repoFilmes, repoSessoes, carrinho, this);
    tela2 = new Tela2AssentosWidget(carrinho, this);
    tela3 = new Tela3BomboniereWidget(repoBomboniere, carrinho, this);
    tela4 = new Tela4PagamentoWidget(carrinho, this);

    stackedWidget->addWidget(telaInicial);
    stackedWidget->addWidget(tela1);
    stackedWidget->addWidget(tela2);
    stackedWidget->addWidget(tela3);
    stackedWidget->addWidget(tela4);

#ifdef QT_DEBUG
    // Atalhos de navegacao so para desenvolvimento; nao existem na build Release.
    new QShortcut(QKeySequence(Qt::Key_F1), this, [this]() { stackedWidget->setCurrentWidget(tela1); });
    new QShortcut(QKeySequence(Qt::Key_F2), this, [this]() { 
        if (repoSessoes.tamanho() > 0) {
            carrinho.limparTudo();
            carrinho.definirSessao(&repoSessoes.obter(0));
            carrinho.adicionarIngresso(std::make_unique<IngressoInteira>(32.0));
            tela2->carregarSessaoAtual();
        }
        stackedWidget->setCurrentWidget(tela2); 
    });
    new QShortcut(QKeySequence(Qt::Key_F3), this, [this]() { stackedWidget->setCurrentWidget(tela3); });
    new QShortcut(QKeySequence(Qt::Key_F4), this, [this]() { stackedWidget->setCurrentWidget(tela4); });
#endif

    // Escolha na tela inicial: cada fluxo comeca com o carrinho limpo.
    connect(telaInicial, &TelaInicialWidget::iniciarCompraCompleta, this, [this]() {
        carrinho.limparTudo();
        tela1->resetar();
        stackedWidget->setCurrentWidget(tela1);
    });

    connect(telaInicial, &TelaInicialWidget::iniciarSomenteBomboniere, this, [this]() {
        carrinho.limparTudo();   // sem sessao e sem ingressos
        tela3->atualizarTela();
        stackedWidget->setCurrentWidget(tela3);
    });

    connect(tela1, &Tela1SelecaoFilmeWidget::voltarParaInicio, this, [this]() {
        tela1->resetar();
        stackedWidget->setCurrentWidget(telaInicial);
    });

    connect(tela3, &Tela3BomboniereWidget::voltarParaInicio, this, [this]() {
        carrinho.limparTudo();
        stackedWidget->setCurrentWidget(telaInicial);
    });

    connect(tela1, &Tela1SelecaoFilmeWidget::avancarParaAssentos, this, [this]() {
        tela2->carregarSessaoAtual();
        stackedWidget->setCurrentWidget(tela2);
    });

    connect(tela2, &Tela2AssentosWidget::voltarParaFilmes, this, [this]() {
        stackedWidget->setCurrentWidget(tela1);
    });

    connect(tela2, &Tela2AssentosWidget::avancarParaBomboniere, this, [this]() {
        tela3->atualizarTela();
        stackedWidget->setCurrentWidget(tela3);
    });

    connect(tela3, &Tela3BomboniereWidget::voltarParaAssentos, this, [this]() {
        tela2->carregarSessaoAtual();
        stackedWidget->setCurrentWidget(tela2);
    });

    connect(tela3, &Tela3BomboniereWidget::avancarParaPagamento, this, [this]() {
        tela4->carregarResumo();
        stackedWidget->setCurrentWidget(tela4);
    });

    connect(tela4, &Tela4PagamentoWidget::voltarParaBomboniere, this, [this]() {
        tela3->atualizarTela();
        stackedWidget->setCurrentWidget(tela3);
    });

    connect(tela4, &Tela4PagamentoWidget::compraFinalizadaComSucesso, this, [this]() {
        tela1->resetar();
        stackedWidget->setCurrentWidget(telaInicial);
    });

    QList<QPushButton*> todosBotoes = this->findChildren<QPushButton*>();
    for (QPushButton* btn : todosBotoes) {
        btn->setCursor(Qt::PointingHandCursor);
    }

    // So agora todos os ponteiros de tela estao validos.
    std::thread(&MainWindow::iniciarModoAdminConsole, this).detach();
}

namespace {

// Limites e valores padrao ao cadastrar uma sala nova.
constexpr int kFileirasPadrao = 6;
constexpr int kColunasPadrao  = 8;
constexpr int kMaxFileiras    = 26;  // identificadores de assento vao de A ate Z
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
    TecnologiaExibicao tecnologia = TecnologiaExibicao::_2D;
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
    out.tecnologia = (escolha == 1) ? TecnologiaExibicao::_2D : TecnologiaExibicao::_3D;

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

} // namespace

// Roda em uma thread separada e so faz I/O de console. Toda leitura/escrita nos
// repositorios e na GUI e despachada para a thread principal via fila de eventos.
void MainWindow::iniciarModoAdminConsole() {
    QPointer<MainWindow> self(this);

    // Copia os dados que o dialogo precisa; bloqueia a thread do admin (nunca a da GUI).
    auto tirarSnapshot = [self]() {
        SnapshotAdmin snap;
        if (!self) return snap;
        QMetaObject::invokeMethod(self.data(), [self, &snap]() {
            if (!self) return;
            for (size_t i = 0; i < self->repoFilmes.tamanho(); ++i) {
                const Filme& f = self->repoFilmes.obter(i);
                snap.filmes.emplace_back(f.getId(), f.getTitulo());
            }
            for (size_t i = 0; i < self->repoSessoes.tamanho(); ++i) {
                const Sessao& s = self->repoSessoes.obter(i);
                const Sala& sala = s.getSala();
                snap.salas[sala.getNumero()] = SalaInfo{sala.getNumero(), sala.getTipoSala(),
                                                       sala.getQuantidadeFileiras(), sala.getQuantidadeColunas()};
                snap.ocupacao.insert({sala.getNumero(), s.getHorario()});
            }
        }, Qt::BlockingQueuedConnection);
        return snap;
    };

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
        if (!self) break; // janela ja foi destruida

        if (opcao == "1") {
            std::string titulo, genero;
            int duracao = 0, classificacao = 0;

            if (!lerLinha("Titulo do filme: ", titulo)) break;
            if (titulo.empty()) { std::cout << "[ADMIN] Titulo vazio. Cancelado.\n"; continue; }
            if (!lerInteiro("Duracao (min): ", 1, 1000, duracao)) break;
            if (!lerInteiro("Classificacao (anos, 0 = livre): ", 0, 18, classificacao)) break;
            if (!lerLinha("Genero: ", genero)) break;
            if (!self) break;

            // O Filme usa um contador estatico de id: criar na thread da GUI.
            QMetaObject::invokeMethod(self.data(), [self, titulo, duracao, classificacao, genero]() {
                if (!self) return;
                Filme novo(titulo, duracao, classificacao, genero);
                const int idNovo = novo.getId();
                self->repoFilmes.adicionar(novo);
                self->tela1->atualizarCatalogo();
                std::cout << "[ADMIN] Filme adicionado com sucesso! (ID " << idNovo << ")\n" << std::flush;
            }, Qt::QueuedConnection);
        }
        else if (opcao == "2") {
            std::string nome;
            double preco = 0.0;

            if (!lerLinha("Nome do produto: ", nome)) break;
            if (nome.empty()) { std::cout << "[ADMIN] Nome vazio. Cancelado.\n"; continue; }
            if (!lerDecimal("Preco (R$): ", 0.0, preco)) break;
            if (!self) break;

            QMetaObject::invokeMethod(self.data(), [self, nome, preco]() {
                if (!self) return;
                // O ItemBomboniere usa um contador estatico de id: criar na thread da GUI.
                self->repoBomboniere.adicionar(ItemBomboniere(nome, preco, ""));
                self->tela3->atualizarTela();
                std::cout << "[ADMIN] Produto de bomboniere adicionado com sucesso!\n" << std::flush;
            }, Qt::QueuedConnection);
        }
        else if (opcao == "3") {
            QMetaObject::invokeMethod(self.data(), [self]() {
                if (!self) return;
                std::cout << "\n================ [RELATORIO DO SISTEMA] ================\n";

                std::cout << "\n--- FILMES CADASTRADOS ---\n";
                for (size_t i = 0; i < self->repoFilmes.tamanho(); ++i) {
                    const Filme& f = self->repoFilmes.obter(i);
                    std::cout << " * [ID " << f.getId() << "] " << f.getTitulo() << " (" << f.getGenero() << ", " << f.getDuracao() << " min)\n";
                }

                std::cout << "\n--- SESSOES DISPONIVEIS ---\n";
                for (size_t i = 0; i < self->repoSessoes.tamanho(); ++i) {
                    const Sessao& s = self->repoSessoes.obter(i);
                    std::cout << " * [Sessao ID " << s.getId() << "] Filme: " << s.getFilme().getTitulo()
                              << " | Horario: " << s.getHorario() << " | Preco Base: R$ " << s.getPrecoBase() << "\n";
                }

                std::cout << "\n--- PRODUTOS DA BOMBONIERE ---\n";
                for (size_t i = 0; i < self->repoBomboniere.tamanho(); ++i) {
                    const ItemBomboniere& item = self->repoBomboniere.obter(i);
                    std::cout << " * [ID " << item.getId() << "] " << item.getNome() << " - R$ " << item.getPreco() << "\n";
                }
                std::cout << "=========================================================\n\n" << std::flush;
            }, Qt::QueuedConnection);
        }
        else if (opcao == "4") {
            const SnapshotAdmin snap = tirarSnapshot();
            if (!self) break;
            if (snap.filmes.empty()) {
                std::cout << "[ADMIN] Nenhum filme cadastrado. Cadastre um filme antes (opcao 1).\n";
                continue;
            }

            NovaSessao nova;
            const ResultadoDialogo r = coletarNovaSessao(snap, nova);
            if (r == ResultadoDialogo::STDIN_FECHADO) break;
            if (r == ResultadoDialogo::CANCELADO) { std::cout << "[ADMIN] Cadastro de sessao cancelado.\n"; continue; }
            if (!self) break;

            QMetaObject::invokeMethod(self.data(), [self, nova]() {
                if (!self) return;

                const Filme* filme = nullptr;
                int novoId = 1;
                for (size_t i = 0; i < self->repoFilmes.tamanho(); ++i) {
                    if (self->repoFilmes.obter(i).getId() == nova.idFilme) { filme = &self->repoFilmes.obter(i); break; }
                }
                for (size_t i = 0; i < self->repoSessoes.tamanho(); ++i) {
                    novoId = std::max(novoId, self->repoSessoes.obter(i).getId() + 1);
                }
                if (!filme) {
                    std::cout << "[ADMIN] Filme nao encontrado. Sessao nao cadastrada.\n" << std::flush;
                    return;
                }

                try {
                    // Sessao guarda copias de Filme e Sala: cada sessao tem a sua propria lotacao.
                    self->repoSessoes.adicionar(Sessao(novoId, *filme,
                        Sala(nova.numeroSala, nova.fileiras, nova.colunas, nova.tipoSala),
                        nova.horario, nova.audio, nova.tecnologia, nova.preco));
                } catch (const std::exception& e) {
                    std::cout << "[ADMIN] Nao foi possivel criar a sessao: " << e.what() << "\n" << std::flush;
                    return;
                }
                self->tela1->atualizarCatalogo();
                std::cout << "[ADMIN] Sessao ID " << novoId << " adicionada para \"" << filme->getTitulo() << "\".\n" << std::flush;
            }, Qt::QueuedConnection);
        }
        else if (!opcao.empty()) {
            std::cout << "[ADMIN] Comando desconhecido: " << opcao << "\n";
        }
    }
}

void MainWindow::inicializarDados() {
    Filme f1("Duna: Parte 2", 166, 14, "Ficção Científica");
    Filme f2("Interestelar", 169, 10, "Ficção / Drama");
    Filme f3("Oppenheimer", 180, 16, "Biografia / Drama");
    Filme f4("Avatar: O Caminho da Água", 192, 12, "Ficção / Aventura");
    Filme f5("A Origem", 148, 12, "Ficção / Ação");
    Filme f6("Matrix", 136, 14, "Ficção / Ação");

    repoFilmes.adicionar(f1);
    repoFilmes.adicionar(f2);
    repoFilmes.adicionar(f3);
    repoFilmes.adicionar(f4);
    repoFilmes.adicionar(f5);
    repoFilmes.adicionar(f6);

    Sala sala1(1, 6, 8, "Padrão");
    Sala sala2(2, 6, 8, "3D");
    Sala sala3(3, 6, 8, "Padrão");
    Sala salaVIP(4, 6, 8, "VIP");

    //Duna
    repoSessoes.adicionar(Sessao(1, f1, sala1, "14:30", TipoAudio::DUBLADO, TecnologiaExibicao::_2D, 32.0));
    repoSessoes.adicionar(Sessao(2, f1, sala2, "18:00", TipoAudio::LEGENDADO, TecnologiaExibicao::_3D, 40.0));
    repoSessoes.adicionar(Sessao(3, f1, salaVIP, "21:15", TipoAudio::LEGENDADO, TecnologiaExibicao::_2D, 50.0));

    //Interestelar
    repoSessoes.adicionar(Sessao(4, f2, sala1, "16:00", TipoAudio::LEGENDADO, TecnologiaExibicao::_2D, 30.0));
    repoSessoes.adicionar(Sessao(5, f2, salaVIP, "20:00", TipoAudio::LEGENDADO, TecnologiaExibicao::_2D, 48.0));

    //Oppenheimer
    repoSessoes.adicionar(Sessao(6, f3, sala2, "19:30", TipoAudio::LEGENDADO, TecnologiaExibicao::_2D, 34.0));
    repoSessoes.adicionar(Sessao(7, f3, sala3, "22:00", TipoAudio::DUBLADO, TecnologiaExibicao::_2D, 32.0));

    //Avatar
    repoSessoes.adicionar(Sessao(8, f4, sala2, "15:00", TipoAudio::DUBLADO, TecnologiaExibicao::_3D, 40.0));
    repoSessoes.adicionar(Sessao(9, f4, salaVIP, "19:00", TipoAudio::LEGENDADO, TecnologiaExibicao::_3D, 55.0));

    //A Origem
    repoSessoes.adicionar(Sessao(10, f5, sala1, "17:00", TipoAudio::LEGENDADO, TecnologiaExibicao::_2D, 30.0));
    repoSessoes.adicionar(Sessao(11, f5, salaVIP, "21:00", TipoAudio::DUBLADO, TecnologiaExibicao::_2D, 48.0));

    //Matrix
    repoSessoes.adicionar(Sessao(12, f6, sala1, "15:00", TipoAudio::LEGENDADO, TecnologiaExibicao::_2D, 30.0));
    repoSessoes.adicionar(Sessao(13, f6, salaVIP, "19:00", TipoAudio::DUBLADO, TecnologiaExibicao::_2D, 48.0));

    repoBomboniere.adicionar(ItemBomboniere(1, "Pipoca Grande", 26.0, ":/img/pipoca.png"));
    repoBomboniere.adicionar(ItemBomboniere(2, "Pipoca Média", 14.0, ":/img/pipoca2.png"));
    repoBomboniere.adicionar(ItemBomboniere(3, "Refrigerante 700ml", 14.0, ":/img/refri.png"));
    repoBomboniere.adicionar(ItemBomboniere(4, "Refrigerante 500ml", 10.0, ":/img/refri.png"));
    repoBomboniere.adicionar(ItemBomboniere(5, "Chocolate Barra", 10.0, ":/img/chocolate.png"));
    repoBomboniere.adicionar(ItemBomboniere(6, "Nachos com Cheddar", 24.0, ":/img/nachos.png"));
    repoBomboniere.adicionar(ItemBomboniere(7, "Agua Mineral 500ml", 7.0, ":/img/agua.png"));
    repoBomboniere.adicionar(ItemBomboniere(8, "Bala de Goma", 6.0, ":/img/bala.png"));

    carrinho.cadastrarRegraCombo({"Combo Solo", {1, 3}, 4.0});
    carrinho.cadastrarRegraCombo({"Combo Casal", {1, 1, 3, 3}, 10.0});
}