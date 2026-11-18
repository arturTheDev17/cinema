/**
 * @file BarraTituloWidget.cpp
 * @brief Implementação da barra de título personalizada BarraTituloWidget.
 */

#include "Estilo.h"
#include "BarraTituloWidget.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPixmap>
#include <QWindow>

/** @details Configura altura fixa, ícone, título, botões de controle de janela, estilo QSS e sinais. */
BarraTituloWidget::BarraTituloWidget(const QString& titulo, QWidget* parent) : QFrame(parent) {
    setObjectName("barraTitulo");
    setFixedHeight(36);

    lblIcone = new QLabel(this);
    const QPixmap logo(":/logo.png");
    if (!logo.isNull()) {
        lblIcone->setPixmap(logo.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    lblTitulo = new QLabel(titulo, this);
    // Cliques no texto e no ícone devem chegar na barra (para arrastar a janela).
    lblIcone->setAttribute(Qt::WA_TransparentForMouseEvents);
    lblTitulo->setAttribute(Qt::WA_TransparentForMouseEvents);

    auto criarBotao = [this](const QString& texto, const QString& dica) {
        QPushButton* b = new QPushButton(texto, this);
        b->setToolTip(dica);
        b->setFixedSize(46, 36);
        b->setFocusPolicy(Qt::NoFocus);
        return b;
    };
    btnMinimizar = criarBotao(QString::fromUtf8("\u2212"), "Minimizar");
    btnMaximizar = criarBotao(QString::fromUtf8("\u25A1"), "Maximizar");
    btnFechar    = criarBotao(QString::fromUtf8("\u00D7"), "Fechar");
    btnFechar->setObjectName("btnFecharJanela");

    QHBoxLayout* lay = new QHBoxLayout(this);
    lay->setContentsMargins(10, 0, 0, 0);
    lay->setSpacing(8);
    lay->addWidget(lblIcone);
    lay->addWidget(lblTitulo);
    lay->addStretch(1);
    lay->addWidget(btnMinimizar);
    lay->addWidget(btnMaximizar);
    lay->addWidget(btnFechar);

    setStyleSheet(
        "#barraTitulo { background-color: " COR_BARRA "; border-bottom: 1px solid " COR_BORDA "; }"
        "#barraTitulo QLabel { background: transparent; color: " COR_TEXTO "; font-weight: bold; font-size: 13px; }"
        "#barraTitulo QPushButton { background: transparent; color: " COR_TEXTO "; border: none; border-radius: 0px; font-size: 18px; }"
        "#barraTitulo QPushButton:hover { background-color: " COR_BORDA "; color: " COR_TEXTO "; }"
        "#barraTitulo QPushButton#btnFecharJanela:hover { background-color: " COR_ERRO "; color: " COR_TEXTO "; }");

    connect(btnMinimizar, &QPushButton::clicked, this, &BarraTituloWidget::minimizarSolicitado);
    connect(btnMaximizar, &QPushButton::clicked, this, &BarraTituloWidget::alternarMaximizarSolicitado);
    connect(btnFechar, &QPushButton::clicked, this, &BarraTituloWidget::fecharSolicitado);
}

/** @details Alterna o glifo e o tooltip do botão de maximizar/restaurar. */
void BarraTituloWidget::definirMaximizado(bool maximizado) {
    btnMaximizar->setText(QString::fromUtf8(maximizado ? "\u2750" : "\u25A1"));
    btnMaximizar->setToolTip(maximizado ? "Restaurar" : "Maximizar");
}

/** @details Delega ao gerenciador de janelas do sistema o movimento nativo via startSystemMove(). */
void BarraTituloWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        // O sistema move a janela (multiplataforma); funciona também com Aero Snap/etc.
        if (QWindow* janela = window()->windowHandle()) {
            janela->startSystemMove();
            return;
        }
    }
    QFrame::mousePressEvent(event);
}

/** @details Emite alternarMaximizarSolicitado() quando recebe duplo clique esquerdo. */
void BarraTituloWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit alternarMaximizarSolicitado();
        return;
    }
    QFrame::mouseDoubleClickEvent(event);
}