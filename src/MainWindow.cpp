#include "MainWindow.h"
#include <iostream>  
#include <thread>
#ifdef _WIN32
#include <windows.h>
#include <dwmapi.h>
#include <QShortcut>

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
        
    std::thread(&MainWindow::iniciarModoAdminConsole, this).detach();

#ifdef _WIN32
    HWND hwnd = reinterpret_cast<HWND>(this->winId());
    BOOL useDarkMode = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));
#endif

    inicializarDados();

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    tela1 = new Tela1SelecaoFilmeWidget(repoFilmes, repoSessoes, carrinho, this);
    tela2 = new Tela2AssentosWidget(carrinho, this);
    tela3 = new Tela3BomboniereWidget(repoBomboniere, carrinho, this);
    tela4 = new Tela4PagamentoWidget(carrinho, this);

    stackedWidget->addWidget(tela1);
    stackedWidget->addWidget(tela2);
    stackedWidget->addWidget(tela3);
    stackedWidget->addWidget(tela4);

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
        stackedWidget->setCurrentWidget(tela1);
    });

    QList<QPushButton*> todosBotoes = this->findChildren<QPushButton*>();
    for (QPushButton* btn : todosBotoes) {
        btn->setCursor(Qt::PointingHandCursor);
    }
}

void MainWindow::iniciarModoAdminConsole() {
    std::cout << "========================================\n";
    std::cout << " [MODO ADMIN CONSOLE ATIVO]\n";
    std::cout << " Comandos disponiveis:\n";
    std::cout << "   1 - Adicionar Filme\n";
    std::cout << "   2 - Adicionar Produto Bomboniere\n";
    std::cout << "   3 - Listar Tudo (Filmes, Sessoes e Bomboniere)\n";
    std::cout << "========================================\n";

    int opcao = 0;
    while (true) {
        std::cin >> opcao;
        if (opcao == 1) {
            std::string titulo, genero;
            int duracao, classificacao;
            
            std::cout << "Titulo do filme: ";
            std::cin.ignore(); 
            std::getline(std::cin, titulo);
            
            std::cout << "Duracao (min): ";
            std::cin >> duracao;
            
            std::cout << "Classificacao (anos): ";
            std::cin >> classificacao;
            
            std::cout << "Genero: ";
            std::cin.ignore(); 
            std::getline(std::cin, genero);

            repoFilmes.adicionar(Filme(titulo, duracao, classificacao, genero));
            std::cout << "[ADMIN] Filme adicionado com sucesso!\n";
        } 
        else if (opcao == 2) {
            std::string nome;
            double preco;
            
            std::cout << "Nome do produto: ";
            std::cin.ignore(); 
            std::getline(std::cin, nome);
            
            std::cout << "Preco (R$): ";
            std::cin >> preco;

            repoBomboniere.adicionar(ItemBomboniere(nome, preco, ""));
            std::cout << "[ADMIN] Produto de bomboniere adicionado com sucesso!\n";

            QMetaObject::invokeMethod(tela3, [this]() {
                tela3->atualizarTela();
            }, Qt::QueuedConnection);
        } 
        else if (opcao == 3) {
            std::cout << "\n================ [RELATORIO DO SISTEMA] ================\n";
            
            std::cout << "\n--- FILMES CADASTRADOS ---\n";
            for (size_t i = 0; i < repoFilmes.tamanho(); ++i) {
                const Filme& f = repoFilmes.obter(i);
                std::cout << " * " << f.getTitulo() << " (" << f.getGenero() << ", " << f.getDuracao() << " min)\n";
            }

            std::cout << "\n--- SESSOES DISPONIVEIS ---\n";
            for (size_t i = 0; i < repoSessoes.tamanho(); ++i) {
                const Sessao& s = repoSessoes.obter(i);
                std::cout << " * [Sessao ID " << s.getId() << "] Filme: " << s.getFilme().getTitulo() 
                          << " | Horario: " << s.getHorario() << " | Preco Base: R$ " << s.getPrecoBase() << "\n";
            }

            std::cout << "\n--- PRODUTOS DA BOMBONIERE ---\n";
            for (size_t i = 0; i < repoBomboniere.tamanho(); ++i) {
                const ItemBomboniere& item = repoBomboniere.obter(i);
                std::cout << " * [ID " << item.getId() << "] " << item.getNome() << " - R$ " << item.getPreco() << "\n";
            }
            std::cout << "=========================================================\n\n";
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
    repoSessoes.adicionar(Sessao(8, f5, sala1, "17:00", TipoAudio::LEGENDADO, TecnologiaExibicao::_2D, 30.0));
    repoSessoes.adicionar(Sessao(9, f5, salaVIP, "21:00", TipoAudio::DUBLADO, TecnologiaExibicao::_2D, 48.0));

    //Matrix
    repoSessoes.adicionar(Sessao(10, f6, sala1, "15:00", TipoAudio::LEGENDADO, TecnologiaExibicao::_2D, 30.0));
    repoSessoes.adicionar(Sessao(11, f6, salaVIP, "19:00", TipoAudio::DUBLADO, TecnologiaExibicao::_2D, 48.0));

    repoBomboniere.adicionar(ItemBomboniere(1, "Pipoca Grande", 26.0, ":/img/pipoca.png"));
    repoBomboniere.adicionar(ItemBomboniere(2, "Pipoca Média", 14.0, ":/img/pipoca2.png"));
    repoBomboniere.adicionar(ItemBomboniere(3, "Refrigerante 700ml", 14.0, ":/img/refri.png"));
    repoBomboniere.adicionar(ItemBomboniere(4, "Refrigerante 500ml", 10.0, ":/img/refri.png"));
    repoBomboniere.adicionar(ItemBomboniere(5, "Chocolate Barra", 10.0, ":/img/chocolate.png"));
    repoBomboniere.adicionar(ItemBomboniere(6, "Nachos com Cheddar", 24.0, ":/img/nachos.png"));
    repoBomboniere.adicionar(ItemBomboniere(7, "Agua Mineral 500ml", 7.0, ":/img/agua.png"));
    repoBomboniere.adicionar(ItemBomboniere(8, "Bala de Goma", 6.0, ":/img/bala.png"));

    carrinho.cadastrarRegraCombo({"Combo Solo", {1, 2}, 4.0});
    carrinho.cadastrarRegraCombo({"Combo Casal", {1, 1, 2, 2}, 10.0});
}