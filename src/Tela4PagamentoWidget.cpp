#include "Tela4PagamentoWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMessageBox>

Tela4PagamentoWidget::Tela4PagamentoWidget(Carrinho& carrinho, QWidget* parent)
    : QWidget(parent), carrinho(carrinho), progressoMs(0) {

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    sidebar = new SidebarCarrinhoWidget(this);
    sidebar->definirModoExpandido(true);

    QWidget* painelPagamento = new QWidget(this);
    QVBoxLayout* pagLayout = new QVBoxLayout(painelPagamento);
    pagLayout->setSpacing(16);

    QGroupBox* grpMetodo = new QGroupBox("Forma de Pagamento", this);
    grpMetodo->setStyleSheet("QGroupBox { border: 1px solid #3d4d52; border-radius: 6px; margin-top: 10px; padding-top: 14px; color: #fbfef9; font-weight: bold; } QGroupBox::title { color: #dec402; }");
    QVBoxLayout* layMetodo = new QVBoxLayout(grpMetodo);

    comboFormaPagamento = new QComboBox(this);
    comboFormaPagamento->addItems({"Cartao de Credito", "Cartao de Debito", "PIX"});
    comboFormaPagamento->setMinimumHeight(42);
    comboFormaPagamento->setStyleSheet(
        "QComboBox {"
        "   background-color: #3d4d52;"
        "   color: #fbfef9;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   padding: 6px 12px;"
        "   border-radius: 6px;"
        "   border: 1px solid #181c25;"
        "}"
        "QComboBox:hover {"
        "   border: 1px solid #dec402;"
        "}"
        "QComboBox::drop-down {"
        "   subcontrol-origin: padding;"
        "   subcontrol-position: top right;"
        "   width: 30px;"
        "   border-left-width: 0px;"
        "   border-top-right-radius: 6px;"
        "   border-bottom-right-radius: 6px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #3d4d52;"
        "   color: #fbfef9;"
        "   selection-background-color: #dec402;"
        "   selection-color: #181c25;"
        "   border: 1px solid #181c25;"
        "   outline: none;"
        "}"
    );
    layMetodo->addWidget(comboFormaPagamento);
    pagLayout->addWidget(grpMetodo);

    lblValorDestaque = new QLabel("Total a Pagar: R$ 0,00", this);
    lblValorDestaque->setStyleSheet("font-size: 22px; font-weight: bold; color: #7FBB3A; padding: 15px 0; background: transparent;");
    lblValorDestaque->setAlignment(Qt::AlignCenter);
    pagLayout->addWidget(lblValorDestaque);

    pagLayout->addStretch();

    btnPagar = new QPushButton("Segure por 2s para Realizar Pagamento", this);
    btnPagar->setMinimumHeight(50);
    btnPagar->setCursor(Qt::PointingHandCursor);
    btnPagar->setStyleSheet("QPushButton { background-color: #7FBB3A; color: #181c25; font-size: 15px; font-weight: bold; border-radius: 6px; border: none; }"
                            "QPushButton:hover { background-color: #dec402; color: #181c25; }");

    barraProgresso = new QProgressBar(this);
    barraProgresso->setRange(0, 2000);
    barraProgresso->setValue(0);
    barraProgresso->setTextVisible(false);
    barraProgresso->setFixedHeight(14);
    barraProgresso->setStyleSheet("QProgressBar { border: 1px solid #3d4d52; border-radius: 4px; background: #181c25; } "
                                  "QProgressBar::chunk { background-color: #7FBB3A; border-radius: 3px; }");

    btnVoltar = new QPushButton("Voltar para Bomboniere", this);
    btnVoltar->setMinimumHeight(42);
    btnVoltar->setCursor(Qt::PointingHandCursor);
    btnVoltar->setStyleSheet("QPushButton { background-color: #3d4d52; color: #fbfef9; font-weight: bold; border-radius: 6px; border: 1px solid #181c25; }"
                             "QPushButton:hover { background-color: #D32222; color: #fbfef9; }");

    pagLayout->addWidget(btnPagar);
    pagLayout->addWidget(barraProgresso);
    pagLayout->addSpacing(8);
    pagLayout->addWidget(btnVoltar);

    mainLayout->addWidget(sidebar, 1);
    mainLayout->addWidget(painelPagamento, 1);

    timerPagamento = new QTimer(this);
    timerPagamento->setInterval(50);

    connect(btnPagar, &QPushButton::pressed, this, &Tela4PagamentoWidget::onBotaoPagarPressionado);
    connect(btnPagar, &QPushButton::released, this, &Tela4PagamentoWidget::onBotaoPagarSolto);
    connect(timerPagamento, &QTimer::timeout, this, &Tela4PagamentoWidget::onTimerTick);
    connect(btnVoltar, &QPushButton::clicked, this, &Tela4PagamentoWidget::voltarParaBomboniere);
}

void Tela4PagamentoWidget::carregarResumo() {
    progressoMs = 0;
    barraProgresso->setValue(0);
    btnPagar->setText("Segure por 2s para Realizar Pagamento");
    sidebar->atualizar(carrinho, true);
    lblValorDestaque->setText(QString("Total a Pagar: R$ %1").arg(carrinho.calcularTotalFinal(), 0, 'f', 2));
}

void Tela4PagamentoWidget::onBotaoPagarPressionado() {
    progressoMs = 0;
    barraProgresso->setValue(0);
    btnPagar->setText("Mantenha pressionado...");
    timerPagamento->start();
}

void Tela4PagamentoWidget::onBotaoPagarSolto() {
    if (progressoMs < 2000) {
        timerPagamento->stop();
        progressoMs = 0;
        barraProgresso->setValue(0);
        btnPagar->setText("Segure por 2s para Realizar Pagamento");
    }
}

void Tela4PagamentoWidget::onTimerTick() {
    progressoMs += 50;
    barraProgresso->setValue(progressoMs);

    if (progressoMs >= 2000) {
        timerPagamento->stop();
        double valorPago = carrinho.calcularTotalFinal();
        carrinho.finalizarCompra();

        QMessageBox::information(this, "Compra Confirmada",
                                  QString("Pagamento de R$ %1 realizado com sucesso!\nSeus assentos foram bloqueados.")
                                      .arg(valorPago, 0, 'f', 2));

        progressoMs = 0;
        barraProgresso->setValue(0);
        btnPagar->setText("Segure por 2s para Realizar Pagamento");
        emit compraFinalizadaComSucesso();
    }
}