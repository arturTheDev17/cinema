#include "Tela3BomboniereWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QGroupBox>
#include <QPixmap>

Tela3BomboniereWidget::Tela3BomboniereWidget(RepositorioGenerico<ItemBomboniere>& bomboniere,
                                           Carrinho& carrinho,
                                           QWidget* parent)
    : QWidget(parent), repoBomboniere(bomboniere), carrinho(carrinho) {

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    catalogoLayout = new QVBoxLayout();
    catalogoLayout->setSpacing(12);

    QGroupBox* grpCombos = new QGroupBox("Combos", this);
    grpCombos->setStyleSheet("QGroupBox { border: 1px solid #3d4d52; border-radius: 3px; margin-top: 12px; padding-top: 10px; color: #fbfef9; font-weight: bold; } QGroupBox::title { color: #dec402; }");
    QHBoxLayout* layCombos = new QHBoxLayout(grpCombos);
    layCombos->setSpacing(10);
    layCombos->setContentsMargins(10, 20, 10, 10);

    QPushButton* btnComboCasal = new QPushButton("+ Combo Casal (2 Pipocas G + 2 Refris) [-R$ 10,00]", this);
    btnComboCasal->setCursor(Qt::PointingHandCursor);
    btnComboCasal->setStyleSheet("QPushButton { background-color: #ff9800; color: #181c25; font-weight: bold; padding: 10px; border-radius: 6px; border: none; } QPushButton:hover { background-color: #dec402; color: #181c25; }");
    connect(btnComboCasal, &QPushButton::clicked, this, [this]() {
        onAdicionarCombo({1, 1, 2, 2});
    });

    QPushButton* btnComboSolo = new QPushButton("+ Combo Solo (1 Pipoca G + 1 Refri) [-R$ 4,00]", this);
    btnComboSolo->setCursor(Qt::PointingHandCursor);
    btnComboSolo->setStyleSheet("QPushButton { background-color: #ff9800; color: #181c25; font-weight: bold; padding: 10px; border-radius: 6px; border: none; } QPushButton:hover { background-color: #dec402; color: #181c25; }");
    connect(btnComboSolo, &QPushButton::clicked, this, [this]() {
        onAdicionarCombo({1, 2});
    });

    layCombos->addWidget(btnComboCasal);
    layCombos->addWidget(btnComboSolo);
    catalogoLayout->addWidget(grpCombos);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("background: transparent; border: none;");
    catalogoLayout->addWidget(scrollArea, 1);

    QHBoxLayout* navLayout = new QHBoxLayout();
    btnVoltar = new QPushButton("Voltar para Assentos", this);
    btnVoltar->setMinimumHeight(45);
    btnVoltar->setCursor(Qt::PointingHandCursor);

    btnAvancar = new QPushButton("Ir para Pagamento", this);
    btnAvancar->setMinimumHeight(45);
    btnAvancar->setCursor(Qt::PointingHandCursor);

    navLayout->addWidget(btnVoltar);
    navLayout->addWidget(btnAvancar);
    catalogoLayout->addLayout(navLayout);

    sidebar = new SidebarCarrinhoWidget(this);
    sidebar->definirModoExpandido(false);

    mainLayout->addLayout(catalogoLayout, 2);
    mainLayout->addWidget(sidebar, 1);

    connect(btnVoltar, &QPushButton::clicked, this, &Tela3BomboniereWidget::voltarParaAssentos);
    connect(btnAvancar, &QPushButton::clicked, this, &Tela3BomboniereWidget::avancarParaPagamento);

    reconstruirCatalogo();
}

void Tela3BomboniereWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    atualizarTela();
}

void Tela3BomboniereWidget::reconstruirCatalogo() {
    labelsQuantidade.clear();

    QWidget* containerProdutos = new QWidget(this);
    containerProdutos->setStyleSheet("background: transparent;");
    QGridLayout* gridProdutos = new QGridLayout(containerProdutos);
    gridProdutos->setSpacing(12);

    int col = 0;
    int row = 0;
    for (size_t i = 0; i < repoBomboniere.tamanho(); ++i) {
        const ItemBomboniere& item = repoBomboniere.obter(i);
        int idProd = item.getId();

        QFrame* card = new QFrame(this);
        card->setFrameShape(QFrame::StyledPanel);
        card->setStyleSheet("background-color: #3d4d52; border-radius: 8px; border: 1px solid #181c25;");
        card->setFixedSize(210, 245);

        QVBoxLayout* cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(6);
        cardLayout->setContentsMargins(10, 10, 10, 10);

        QLabel* lblImg = new QLabel(this);
        lblImg->setFixedSize(90, 90);
        lblImg->setAlignment(Qt::AlignCenter);
        QPixmap pix(QString::fromStdString(item.getCaminhoImagem()));
        if (!pix.isNull()) {
            lblImg->setPixmap(pix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            lblImg->setText("[FOTO]");
            lblImg->setStyleSheet("background-color: #181c25; border-radius: 6px; font-weight: bold; color: #777777; border: none;");
        }

        QLabel* lblNome = new QLabel(QString::fromStdString(item.getNome()), this);
        lblNome->setAlignment(Qt::AlignCenter);
        lblNome->setStyleSheet("font-weight: bold; font-size: 13px; color: #fbfef9; background: transparent; border: none;");

        QLabel* lblPreco = new QLabel(QString("R$ %1").arg(item.getPreco(), 0, 'f', 2), this);
        lblPreco->setAlignment(Qt::AlignCenter);
        lblPreco->setStyleSheet("color: #dec402; font-weight: bold; font-size: 14px; background: transparent; border: none;");

        QHBoxLayout* ctrlLayout = new QHBoxLayout();
        ctrlLayout->setSpacing(6);
        
        QPushButton* btnMenos = new QPushButton("-", this);
        btnMenos->setFixedSize(32, 32);
        btnMenos->setCursor(Qt::PointingHandCursor);
        btnMenos->setStyleSheet("background-color: #D32222; color: #fbfef9; font-weight: bold; border-radius: 4px; border: none;");
        
        QLabel* lblQtd = new QLabel("0", this);
        lblQtd->setAlignment(Qt::AlignCenter);
        lblQtd->setStyleSheet("font-size: 14px; font-weight: bold; min-width: 24px; color: #fbfef9; background: transparent; border: none;");
        
        QPushButton* btnMais = new QPushButton("+", this);
        btnMais->setFixedSize(32, 32);
        btnMais->setCursor(Qt::PointingHandCursor);
        btnMais->setStyleSheet("background-color: #7FBB3A; color: #181c25; font-weight: bold; border-radius: 4px; border: none;");

        labelsQuantidade[idProd] = lblQtd;

        connect(btnMais, &QPushButton::clicked, this, [this, idProd]() {
            onAdicionarItem(idProd);
        });
        connect(btnMenos, &QPushButton::clicked, this, [this, idProd]() {
            onRemoverItem(idProd);
        });

        ctrlLayout->addWidget(btnMenos);
        ctrlLayout->addWidget(lblQtd);
        ctrlLayout->addWidget(btnMais);

        cardLayout->addWidget(lblImg, 0, Qt::AlignCenter);
        cardLayout->addWidget(lblNome);
        cardLayout->addWidget(lblPreco);
        cardLayout->addLayout(ctrlLayout);

        gridProdutos->addWidget(card, row, col);
        col++;
        if (col >= 3) {
            col = 0;
            row++;
        }
    }

    scrollArea->setWidget(containerProdutos);
}

void Tela3BomboniereWidget::atualizarTela() {
    reconstruirCatalogo(); 
    atualizarContadores();
    sidebar->atualizar(carrinho, true);
}

void Tela3BomboniereWidget::atualizarContadores() {
    for (auto& par : labelsQuantidade) {
        int qtd = carrinho.contarQuantidadeProduto(par.first);
        par.second->setText(QString::number(qtd));
    }
}

void Tela3BomboniereWidget::onAdicionarItem(int idProduto) {
    for (size_t i = 0; i < repoBomboniere.tamanho(); ++i) {
        if (repoBomboniere.obter(i).getId() == idProduto) {
            carrinho.adicionarItemBomboniere(repoBomboniere.obter(i));
            break;
        }
    }
    atualizarTela();
}

void Tela3BomboniereWidget::onRemoverItem(int idProduto) {
    carrinho.removerItemBombonierePorId(idProduto);
    atualizarTela();
}

void Tela3BomboniereWidget::onAdicionarCombo(const std::vector<int>& idsProdutos) {
    for (int id : idsProdutos) {
        for (size_t i = 0; i < repoBomboniere.tamanho(); ++i) {
            if (repoBomboniere.obter(i).getId() == id) {
                carrinho.adicionarItemBomboniere(repoBomboniere.obter(i));
                break;
            }
        }
    }
    atualizarTela();
}