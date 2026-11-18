/**
 * @file Tela3BomboniereWidget.cpp
 * @brief Implementação da tela da bomboniere e combos dinâmicos Tela3BomboniereWidget.
 */

#include "Estilo.h"
#include "Tela3BomboniereWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QGroupBox>
#include <QGridLayout>
#include <QPixmap>
#include <QStringList>
#include <algorithm>
#include <utility>
#include "Formatacao.h"

/** @details Monta caixa de combos, área rolável de produtos, navegação inferior, sidebar e catálogo inicial. */
Tela3BomboniereWidget::Tela3BomboniereWidget(RepositorioGenerico<ItemBomboniere>& bomboniere,
                                           Carrinho& carrinho,
                                           QWidget* parent)
    : QWidget(parent), repoBomboniere(bomboniere), carrinho(carrinho) {

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    catalogoLayout = new QVBoxLayout();
    catalogoLayout->setSpacing(12);

    QGroupBox* grpCombos = new QGroupBox("Combos", this);
    grpCombos->setStyleSheet("QGroupBox { border: 1px solid " COR_BORDA "; border-radius: 3px; margin-top: 12px; padding-top: 10px; color: " COR_TEXTO "; font-weight: bold; } QGroupBox::title { color: " COR_DESTAQUE "; }");
    combosLayout = new QHBoxLayout(grpCombos);
    combosLayout->setSpacing(10);
    combosLayout->setContentsMargins(10, 20, 10, 10);
    catalogoLayout->addWidget(grpCombos);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");
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

    connect(btnVoltar, &QPushButton::clicked, this, &Tela3BomboniereWidget::onVoltarClicado);
    connect(btnAvancar, &QPushButton::clicked, this, &Tela3BomboniereWidget::avancarParaPagamento);

    reconstruirCombos();
    reconstruirCatalogo();
}

/** @details Atualiza contadores, sidebar e navegação sempre que a tela volta a ficar visível. */
void Tela3BomboniereWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    atualizarResumo();
}

/** @details Constrói grade de 3 colunas de cards com imagem/placeholder, nome, preço e botões +/-. */
void Tela3BomboniereWidget::reconstruirCatalogo() {
    itensRenderizados = repoBomboniere.tamanho();
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
        // Seletor por nome: sem ele a borda e o fundo do card "vazam" para os QLabels filhos.
        card->setObjectName("cardProduto");
        card->setStyleSheet("QFrame#cardProduto { background-color: " COR_BORDA "; border-radius: 8px; border: 1px solid " COR_FUNDO "; }");
        card->setFixedSize(210, 245);

        QVBoxLayout* cardLayout = new QVBoxLayout(card);
        cardLayout->setSpacing(6);
        cardLayout->setContentsMargins(10, 10, 10, 10);

        QLabel* lblImg = new QLabel(this);
        lblImg->setFixedSize(90, 90);
        lblImg->setAlignment(Qt::AlignCenter);
        lblImg->setStyleSheet("background: transparent; border: none;"); // Imagens são circulares/transparentes
        QPixmap pix(QString::fromStdString(item.getCaminhoImagem()));
        if (!pix.isNull()) {
            lblImg->setPixmap(pix.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            lblImg->setText("[FOTO]");
            lblImg->setStyleSheet("background-color: " COR_FUNDO "; border-radius: 6px; font-weight: bold; color: " COR_DESATIVADO "; border: none;");
        }

        QLabel* lblNome = new QLabel(QString::fromStdString(item.getNome()), this);
        lblNome->setAlignment(Qt::AlignCenter);
        lblNome->setStyleSheet("font-weight: bold; font-size: 13px; color: " COR_TEXTO "; background: transparent; border: none;");

        QLabel* lblPreco = new QLabel(formatarMoeda(item.getPreco()), this);
        lblPreco->setAlignment(Qt::AlignCenter);
        lblPreco->setStyleSheet("color: " COR_DESTAQUE "; font-weight: bold; font-size: 14px; background: transparent; border: none;");

        QHBoxLayout* ctrlLayout = new QHBoxLayout();
        ctrlLayout->setSpacing(6);
        
        QPushButton* btnMenos = new QPushButton("-", this);
        btnMenos->setFixedSize(32, 32);
        btnMenos->setCursor(Qt::PointingHandCursor);
        btnMenos->setStyleSheet("background-color: " COR_ERRO "; color: " COR_TEXTO "; font-weight: bold; border-radius: 4px; border: none;");
        
        QLabel* lblQtd = new QLabel("0", this);
        lblQtd->setAlignment(Qt::AlignCenter);
        lblQtd->setStyleSheet("font-size: 14px; font-weight: bold; min-width: 24px; color: " COR_TEXTO "; background: transparent; border: none;");
        
        QPushButton* btnMais = new QPushButton("+", this);
        btnMais->setFixedSize(32, 32);
        btnMais->setCursor(Qt::PointingHandCursor);
        btnMais->setStyleSheet("background-color: " COR_SUCESSO "; color: " COR_FUNDO "; font-weight: bold; border-radius: 4px; border: none;");

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

/** @details Consulta o produto por ID via repoBomboniere.buscar() e retorna seu nome. */
QString Tela3BomboniereWidget::nomeProduto(int idProduto) const {
    const ItemBomboniere* item = repoBomboniere.buscar(idProduto);
    return item ? QString::fromStdString(item->getNome()) : QString();
}

/** @details Gera um botão por regra de combo cadastrada no Carrinho (fonte única das regras). */
void Tela3BomboniereWidget::reconstruirCombos() {
    QLayoutItem* child;
    while ((child = combosLayout->takeAt(0)) != nullptr) {
        if (QWidget* w = child->widget()) {
            w->hide();
            w->deleteLater();
        }
        delete child;
    }

    for (const Combo& combo : carrinho.getCombos()) {
        // Conta quantas unidades de cada produto o combo exige, mantendo a ordem de aparição.
        std::vector<std::pair<int, int>> contagem;
        for (int id : combo.getIdsProdutos()) {
            auto it = std::find_if(contagem.begin(), contagem.end(),
                                   [id](const std::pair<int, int>& p) { return p.first == id; });
            if (it == contagem.end()) contagem.emplace_back(id, 1);
            else ++it->second;
        }

        QStringList partes;
        bool todosExistem = true;
        for (const auto& par : contagem) {
            const QString nome = nomeProduto(par.first);
            if (nome.isEmpty()) todosExistem = false;
            partes << QString("%1x %2").arg(par.second).arg(nome.isEmpty() ? QString("#%1").arg(par.first) : nome);
        }

        QPushButton* btn = new QPushButton(
            QString("+ %1 (%2) [-%3]")
                .arg(QString::fromStdString(combo.getNome()))
                .arg(partes.join(" + "))
                .arg(formatarMoeda(combo.getDesconto())), this);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setEnabled(todosExistem); // Combo com produto inexistente não pode ser adicionado
        btn->setStyleSheet("QPushButton { background-color: " COR_ALERTA "; color: " COR_FUNDO "; font-weight: bold; padding: 10px; border-radius: 6px; border: none; } "
                           "QPushButton:hover { background-color: " COR_DESTAQUE "; color: " COR_FUNDO "; } "
                           "QPushButton:disabled { background-color: " COR_FUNDO_DESATIVADO "; color: " COR_DESATIVADO "; }");
        const std::vector<int> ids = combo.getIdsProdutos();
        connect(btn, &QPushButton::clicked, this, [this, ids]() { onAdicionarCombo(ids); });
        combosLayout->addWidget(btn);
    }
}

/** @details Invalida itensRenderizados para forçar reconstrução após adição, edição ou remoção no admin. */
void Tela3BomboniereWidget::atualizarCatalogo() {
    itensRenderizados = static_cast<size_t>(-1);
    atualizarTela();
}

/** @details Só recria cards e combos quando o catálogo mudou em relação a itensRenderizados. */
void Tela3BomboniereWidget::atualizarTela() {
    // Só recria cards e combos quando o catálogo mudou (admin cadastrou produto).
    if (repoBomboniere.tamanho() != itensRenderizados) {
        reconstruirCombos();
        reconstruirCatalogo();
    }
    atualizarResumo();
}

/** @details Atualiza os rótulos de quantidade dos cards consultando o Carrinho. */
void Tela3BomboniereWidget::atualizarContadores() {
    for (auto& par : labelsQuantidade) {
        int qtd = carrinho.contarQuantidadeProduto(par.first);
        par.second->setText(QString::number(qtd));
    }
}

/** @details Busca o item por ID e adiciona ao carrinho usando Carrinho::operator+=. */
void Tela3BomboniereWidget::onAdicionarItem(int idProduto) {
    if (const ItemBomboniere* item = repoBomboniere.buscar(idProduto)) {
        carrinho += *item;
    }
    atualizarResumo();
}

/** @details Remove uma unidade do item informado pelo ID e atualiza o resumo. */
void Tela3BomboniereWidget::onRemoverItem(int idProduto) {
    carrinho.removerItemBombonierePorId(idProduto);
    atualizarResumo();
}

/** @details Adiciona em lote todos os produtos componentes de um combo ao carrinho. */
void Tela3BomboniereWidget::onAdicionarCombo(const std::vector<int>& idsProdutos) {
    for (int id : idsProdutos) {
        if (const ItemBomboniere* item = repoBomboniere.buscar(id)) {
            carrinho += *item;
        }
    }
    atualizarResumo();
}

/** @details Atualiza contadores, sidebar e estado dos botões de navegação. */
void Tela3BomboniereWidget::atualizarResumo() {
    atualizarContadores();
    sidebar->atualizar(carrinho, true);
    atualizarNavegacao();
}

/** @details Adapta o botão Voltar ao fluxo ativo e habilita Pagamento apenas se houver itens no carrinho. */
void Tela3BomboniereWidget::atualizarNavegacao() {
    // Sem sessão no carrinho = fluxo "somente bomboniere": o Voltar leva ao início.
    const bool comSessao = carrinho.getSessao() != nullptr;
    btnVoltar->setText(comSessao ? "Voltar para Assentos" : "Voltar ao Início");
    // Não deixa ir ao pagamento com carrinho vazio.
    btnAvancar->setEnabled(!carrinho.estaVazio());
}

/** @details Emite voltarParaAssentos() ou voltarParaInicio() dependendo de haver sessão ativa no carrinho. */
void Tela3BomboniereWidget::onVoltarClicado() {
    if (carrinho.getSessao() != nullptr) {
        emit voltarParaAssentos();
    } else {
        emit voltarParaInicio();
    }
}