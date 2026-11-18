/**
 * @file MainWindow.cpp
 * @brief Implementação da janela principal MainWindow, navegação entre telas e controle de inatividade.
 */

#include "MainWindow.h"
#include "TelaInicialWidget.h"
#include "Tela1SelecaoFilmeWidget.h"
#include "Tela2AssentosWidget.h"
#include "Tela3BomboniereWidget.h"
#include "Tela4PagamentoWidget.h"
#include "DadosIniciais.h"
#include "BarraTituloWidget.h"

#include <QApplication>
#include <QEvent>
#include <QFrame>
#include <QResizeEvent>
#include <QSizeGrip>
#include <QVBoxLayout>
#include <QIcon>
#include <QKeySequence>
#include <QPushButton>
#include <QShortcut>

#include <memory>

/** @details Aplica folha de estilo QSS global definindo cores, fontes, bordas e barras de rolagem. */
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

/**
 * @details Configura janela sem moldura nativa, carrega dados iniciais, instancia as 5 telas,
 *          conecta os fluxos de navegação, instala filtro de inatividade e inicia o AdminConsole.
 */
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowIcon(QIcon(":/logo.png"));
    aplicarEstiloGlobal();
    setWindowTitle("CinemaMecha");
    resize(1100, 720);

    // Janela sem moldura nativa: a barra de título escura é desenhada por nós e é
    // igual em qualquer sistema operacional.
    setWindowFlag(Qt::FramelessWindowHint, true);

    carregarDadosIniciais(repoFilmes, repoSessoes, repoBomboniere, carrinho);

    stackedWidget = new QStackedWidget(this);

    QFrame* container = new QFrame(this);
    container->setObjectName("janelaContainer");
    container->setStyleSheet("#janelaContainer { background-color: #181c25; border: 1px solid #3d4d52; }");
    QVBoxLayout* layContainer = new QVBoxLayout(container);
    layContainer->setContentsMargins(1, 1, 1, 1);
    layContainer->setSpacing(0);

    barraTitulo = new BarraTituloWidget("CinemaMecha", container);
    layContainer->addWidget(barraTitulo);
    layContainer->addWidget(stackedWidget, 1);
    setCentralWidget(container);

    alcaRedimensionar = new QSizeGrip(this);
    alcaRedimensionar->setFixedSize(14, 14);

    connect(barraTitulo, &BarraTituloWidget::minimizarSolicitado, this, &MainWindow::showMinimized);
    connect(barraTitulo, &BarraTituloWidget::alternarMaximizarSolicitado, this, &MainWindow::alternarMaximizar);
    connect(barraTitulo, &BarraTituloWidget::fecharSolicitado, this, &MainWindow::close);
    new QShortcut(QKeySequence(Qt::Key_F11), this, [this]() { alternarTelaCheia(); });

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

#ifndef QT_NO_DEBUG
    // Atalhos de navegação só para desenvolvimento; não existem na build Release.
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

    // Escolha na tela inicial: cada fluxo começa com o carrinho limpo.
    connect(telaInicial, &TelaInicialWidget::iniciarCompraCompleta, this, [this]() {
        carrinho.limparTudo();
        tela1->resetar();
        stackedWidget->setCurrentWidget(tela1);
    });

    connect(telaInicial, &TelaInicialWidget::iniciarSomenteBomboniere, this, [this]() {
        carrinho.limparTudo();   // Sem sessão e sem ingressos
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
        tela2->carregarSessaoAtual(true); // Mantém os assentos já escolhidos
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

    const QList<QPushButton*> todosBotoes = this->findChildren<QPushButton*>();
    for (QPushButton* btn : todosBotoes) {
        btn->setCursor(Qt::PointingHandCursor);
    }

    // Inatividade: qualquer clique, toque ou tecla reinicia a contagem.
    timerInatividade = new QTimer(this);
    timerInatividade->setSingleShot(true);
    timerInatividade->setInterval(kInatividadeMs);
    connect(timerInatividade, &QTimer::timeout, this, &MainWindow::cancelarCompraPorInatividade);
    qApp->installEventFilter(this);
    timerInatividade->start();

    // Console de admin: só notifica a GUI pelos ganchos, sempre na thread principal.
    AdminConsole::Ganchos ganchos;
    ganchos.aoMudarCatalogo   = [this]() { tela1->atualizarCatalogo(); };
    ganchos.aoMudarBomboniere = [this]() { tela3->atualizarCatalogo(); };
    adminConsole = std::make_unique<AdminConsole>(repoFilmes, repoSessoes, repoBomboniere, ganchos);
    adminConsole->iniciar();
}

/** @details Reinicia timerInatividade quando ocorre MouseButtonPress, TouchBegin ou KeyPress. */
bool MainWindow::eventFilter(QObject* alvo, QEvent* evento) {
    switch (evento->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::TouchBegin:
        case QEvent::KeyPress:
            timerInatividade->start(); // Reinicia a contagem
            break;
        default:
            break;
    }
    return QMainWindow::eventFilter(alvo, evento);
}

/** @details Aborta a compra ociosa (se não houver diálogo modal aberto), libera assentos e volta à telaInicial. */
void MainWindow::cancelarCompraPorInatividade() {
    if (stackedWidget->currentWidget() == telaInicial) return;

    // Não interrompe enquanto uma caixa de diálogo estiver aberta; tenta de novo depois.
    if (QApplication::activeModalWidget() != nullptr) {
        timerInatividade->start();
        return;
    }

    tela2->liberarSelecao();   // Assentos escolhidos mas ainda não vinculados
    carrinho.limparTudo();     // Libera os vinculados e esvazia o carrinho
    tela1->resetar();
    stackedWidget->setCurrentWidget(telaInicial);
}

/** @details Alterna entre tela normal e maximizada (ou sai do modo tela cheia) atualizando o botão da barra. */
void MainWindow::alternarMaximizar() {
    if (isFullScreen()) {
        alternarTelaCheia();
    } else if (isMaximized()) {
        showNormal();
        barraTitulo->definirMaximizado(false);
    } else {
        showMaximized();
        barraTitulo->definirMaximizado(true);
    }
}

/** @details Alterna modo tela cheia (F11), ocultando barra de título e alça no modo totem. */
void MainWindow::alternarTelaCheia() {
    if (isFullScreen()) {
        showNormal();
        barraTitulo->show();
        alcaRedimensionar->show();
        barraTitulo->definirMaximizado(false);
    } else {
        barraTitulo->hide();
        alcaRedimensionar->hide();
        showFullScreen();
    }
}

/** @details Mantém a alça QSizeGrip posicionada e visível no canto inferior direito da janela. */
void MainWindow::resizeEvent(QResizeEvent* evento) {
    QMainWindow::resizeEvent(evento);
    // Mantém a alça de redimensionar no canto inferior direito, por cima das telas.
    alcaRedimensionar->move(width() - alcaRedimensionar->width() - 1,
                            height() - alcaRedimensionar->height() - 1);
    alcaRedimensionar->raise();
}