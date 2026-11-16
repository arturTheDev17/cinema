#include "TelaInicialWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>

QPushButton* TelaInicialWidget::criarCard(const QString& texto) {
    // Mesmo visual dos cards de filme da Tela 1, em tamanho maior.
    QPushButton* card = new QPushButton(texto, this);
    card->setFixedSize(300, 230);
    card->setCursor(Qt::PointingHandCursor);
    card->setStyleSheet(
        "QPushButton { background-color: #3d4d52; border: 2px solid #181c25; border-radius: 10px;"
        " font-size: 17px; font-weight: normal; color: #fbfef9; text-align: center; padding: 12px; }"
        "QPushButton:hover { border-color: #dec402; color: #dec402; }");
    return card;
}

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
    lblTitulo->setStyleSheet("color: #dec402; font-size: 40px; font-weight: bold; background: transparent;");

    lblSubtitulo = new QLabel("Bem-vindo! Como você quer começar?", this);
    lblSubtitulo->setAlignment(Qt::AlignCenter);
    lblSubtitulo->setStyleSheet("color: #fbfef9; font-size: 18px; font-weight: normal; background: transparent;");

    btnCompraCompleta = criarCard("🎬\n\nComprar Ingressos\n\nEscolha filme, sessão e assentos\ne depois a bomboniere");
    btnSomenteBomboniere = criarCard("🍿\n\nSomente Bomboniere\n\nPule a escolha de filme\ne vá direto aos produtos");

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
