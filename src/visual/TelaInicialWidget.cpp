/**
 * @file TelaInicialWidget.cpp
 * @brief Implementação da tela inicial de boas-vindas do totem TelaInicialWidget.
 */

#include "Estilo.h"
#include "TelaInicialWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>

/** @details Cria um QPushButton de 300x230 px seguindo o mesmo padrão visual dos cards de filmes. */
QPushButton* TelaInicialWidget::criarCard(const QString& texto) {
    // Mesmo visual dos cards de filme da Tela 1, em tamanho maior.
    QPushButton* card = new QPushButton(texto, this);
    card->setFixedSize(300, 230);
    card->setCursor(Qt::PointingHandCursor);
    card->setStyleSheet(
        "QPushButton { background-color: " COR_BORDA "; border: 2px solid " COR_FUNDO "; border-radius: 10px;"
        " font-size: 17px; font-weight: normal; color: " COR_TEXTO "; text-align: center; padding: 12px; }"
        "QPushButton:hover { border-color: " COR_DESTAQUE "; color: " COR_DESTAQUE "; }");
    return card;
}

/** @details Centraliza logotipo, título, subtítulo e os dois cards de entrada conectando-os aos sinais da tela. */
TelaInicialWidget::TelaInicialWidget(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(14);

    lblLogo = new QLabel(this);
    lblLogo->setAlignment(Qt::AlignCenter);
    lblLogo->setStyleSheet("background: transparent;");
    QPixmap logo(":/logo.png");
    if (!logo.isNull()) {
        lblLogo->setPixmap(logo.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    lblTitulo = new QLabel("CinemaMecha", this);
    lblTitulo->setAlignment(Qt::AlignCenter);
    lblTitulo->setStyleSheet("color: " COR_DESTAQUE "; font-size: 40px; font-weight: bold; background: transparent;");

    lblSubtitulo = new QLabel("Bem-vindo! Como você quer começar?", this);
    lblSubtitulo->setAlignment(Qt::AlignCenter);
    lblSubtitulo->setStyleSheet("color: " COR_TEXTO "; font-size: 18px; font-weight: normal; background: transparent;");

    btnCompraCompleta = criarCard("🎬\n\nComprar Ingressos\n\nEscolha filme, sessão e assentos\ne depois a bomboniere");
    btnSomenteBomboniere = criarCard("🍿\n\Bomboniere\n\nPule a escolha de filme\ne vá direto aos produtos");

    QHBoxLayout* cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(30);
    cardsLayout->addStretch(1);
    cardsLayout->addWidget(btnCompraCompleta);
    cardsLayout->addWidget(btnSomenteBomboniere);
    cardsLayout->addStretch(1);

    mainLayout->addStretch(2);
    mainLayout->addWidget(lblLogo);
    mainLayout->addWidget(lblTitulo);
    mainLayout->addWidget(lblSubtitulo);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(cardsLayout);
    mainLayout->addStretch(3);

    connect(btnCompraCompleta, &QPushButton::clicked, this, &TelaInicialWidget::iniciarCompraCompleta);
    connect(btnSomenteBomboniere, &QPushButton::clicked, this, &TelaInicialWidget::iniciarSomenteBomboniere);
}