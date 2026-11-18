/**
 * @file Tela4PagamentoWidget.cpp
 * @brief Implementação da tela de pagamento simulado Tela4PagamentoWidget.
 */

#include "Estilo.h"
#include "Tela4PagamentoWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include "Formatacao.h"

/** @details Monta o resumo expandido, seletor de forma de pagamento, valor em destaque e barra de 2000 ms. */
Tela4PagamentoWidget::Tela4PagamentoWidget(Carrinho& carrinho, QWidget* parent)
    : QWidget(parent), carrinho(carrinho), progressoMs(0) {

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    sidebar = new SidebarCarrinhoWidget(this);
    sidebar->definirModoExpandido(true);

    QWidget* painelPagamento = new QWidget(this);
    QVBoxLayout* pagLayout = new QVBoxLayout(painelPagamento);
    pagLayout->setSpacing(16);

    QGroupBox* grpMetodo = new QGroupBox("Forma de Pagamento", this);
    grpMetodo->setStyleSheet("QGroupBox { border: 1px solid " COR_BORDA "; border-radius: 6px; margin-top: 10px; padding-top: 14px; color: " COR_TEXTO "; font-weight: bold; } QGroupBox::title { color: " COR_DESTAQUE "; }");
    QVBoxLayout* layMetodo = new QVBoxLayout(grpMetodo);

    comboFormaPagamento = new QComboBox(this);
    comboFormaPagamento->addItems({"Cartão de Crédito", "Cartão de Débito", "PIX"});
    comboFormaPagamento->setMinimumHeight(42);
    comboFormaPagamento->setStyleSheet(
        "QComboBox {"
        "   background-color: " COR_BORDA ";"
        "   color: " COR_TEXTO ";"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   padding: 6px 12px;"
        "   border-radius: 6px;"
        "   border: 1px solid " COR_FUNDO ";"
        "}"
        "QComboBox:hover {"
        "   border: 1px solid " COR_DESTAQUE ";"
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
        "   background-color: " COR_BORDA ";"
        "   color: " COR_TEXTO ";"
        "   selection-background-color: " COR_DESTAQUE ";"
        "   selection-color: " COR_FUNDO ";"
        "   border: 1px solid " COR_FUNDO ";"
        "   outline: none;"
        "}"
    );
    layMetodo->addWidget(comboFormaPagamento);
    pagLayout->addWidget(grpMetodo);

    lblValorDestaque = new QLabel("Total a Pagar: R$ 0,00", this);
    lblValorDestaque->setStyleSheet("font-size: 22px; font-weight: bold; color: " COR_SUCESSO "; padding: 15px 0; background: transparent;");
    lblValorDestaque->setAlignment(Qt::AlignCenter);
    pagLayout->addWidget(lblValorDestaque);

    pagLayout->addStretch();

    btnPagar = new QPushButton("Segure por 2s para Realizar Pagamento", this);
    btnPagar->setMinimumHeight(50);
    btnPagar->setCursor(Qt::PointingHandCursor);
    btnPagar->setStyleSheet("QPushButton { background-color: " COR_SUCESSO "; color: " COR_FUNDO "; font-size: 15px; font-weight: bold; border-radius: 6px; border: none; }"
                            "QPushButton:hover { background-color: " COR_DESTAQUE "; color: " COR_FUNDO "; }");

    barraProgresso = new QProgressBar(this);
    barraProgresso->setRange(0, 2000);
    barraProgresso->setValue(0);
    barraProgresso->setTextVisible(false);
    barraProgresso->setFixedHeight(14);
    barraProgresso->setStyleSheet("QProgressBar { border: 1px solid " COR_BORDA "; border-radius: 4px; background: " COR_FUNDO "; } "
                                  "QProgressBar::chunk { background-color: " COR_SUCESSO "; border-radius: 3px; }");

    btnVoltar = new QPushButton("Voltar para Bomboniere", this);
    btnVoltar->setMinimumHeight(42);
    btnVoltar->setCursor(Qt::PointingHandCursor);
    btnVoltar->setStyleSheet("QPushButton { background-color: " COR_BORDA "; color: " COR_TEXTO "; font-weight: bold; border-radius: 6px; border: 1px solid " COR_FUNDO "; }"
                             "QPushButton:hover { background-color: " COR_DESTAQUE "; color: " COR_FUNDO "; }");

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

/** @details Reinicia barra e texto do botão, atualiza a sidebar, habilita btnPagar e exibe o total líquido. */
void Tela4PagamentoWidget::carregarResumo() {
    progressoMs = 0;
    barraProgresso->setValue(0);
    btnPagar->setText("Segure por 2s para Realizar Pagamento");
    sidebar->atualizar(carrinho, true);
    btnPagar->setEnabled(!carrinho.estaVazio());
    lblValorDestaque->setText(QString("Total a Pagar: %1").arg(formatarMoeda(carrinho.calcularTotalFinal())));
}

/** @details Zera o contador e inicia timerPagamento em intervalos de 50 ms. */
void Tela4PagamentoWidget::onBotaoPagarPressionado() {
    progressoMs = 0;
    barraProgresso->setValue(0);
    btnPagar->setText("Mantenha pressionado...");
    timerPagamento->start();
}

/** @details Para o temporizador e reseta a barra caso o botão seja solto antes dos 2000 ms. */
void Tela4PagamentoWidget::onBotaoPagarSolto() {
    if (progressoMs < 2000) {
        timerPagamento->stop();
        progressoMs = 0;
        barraProgresso->setValue(0);
        btnPagar->setText("Segure por 2s para Realizar Pagamento");
    }
}

/** @details Incrementa 50 ms por tick; ao atingir 2000 ms, finaliza a compra, exibe confirmação e emite conclusão. */
void Tela4PagamentoWidget::onTimerTick() {
    progressoMs += 50;
    barraProgresso->setValue(progressoMs);

    if (progressoMs >= 2000) {
        timerPagamento->stop();
        const double valorPago = carrinho.calcularTotalFinal();
        const QString forma = comboFormaPagamento->currentText(); // Lida antes de esvaziar o carrinho
        const bool temAssentos = carrinho.getQuantidadeIngressos() > 0; // finalizarCompra() esvazia o carrinho
        carrinho.finalizarCompra();

        QMessageBox::information(this, "Compra Confirmada",
                                  QString("Pagamento de %1 (%2) realizado com sucesso!%3")
                                      .arg(formatarMoeda(valorPago))
                                      .arg(forma)
                                      .arg(temAssentos ? "\nSeus assentos foram reservados." : ""));

        progressoMs = 0;
        barraProgresso->setValue(0);
        btnPagar->setText("Segure por 2s para Realizar Pagamento");
        emit compraFinalizadaComSucesso();
    }
}