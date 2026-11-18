/**
 * @file SidebarCarrinhoWidget.cpp
 * @brief Implementação do painel lateral de resumo do pedido SidebarCarrinhoWidget.
 */

#include "Estilo.h"
#include "SidebarCarrinhoWidget.h"
#include <QFont>
#include <QVBoxLayout>
#include "Formatacao.h"

/** @details Monta título, informações da sessão, lista de itens, aviso vazio, combos aplicados e total. */
SidebarCarrinhoWidget::SidebarCarrinhoWidget(QWidget* parent) : QWidget(parent) {
    setMinimumWidth(300);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(12);

    lblTitulo = new QLabel("RESUMO DO PEDIDO", this);
    QFont fontTitulo = lblTitulo->font();
    fontTitulo.setBold(true);
    fontTitulo.setPointSize(14);
    lblTitulo->setFont(fontTitulo);
    lblTitulo->setAlignment(Qt::AlignCenter);
    lblTitulo->setStyleSheet("color: " COR_DESTAQUE "; background: transparent; border: none;");

    QString estiloCaixaInterna = "color: " COR_TEXTO "; font-size: 14px; background: transparent; padding: 16px; border: 1px solid " COR_BORDA "; border-radius: 3px;";

    lblInfoSessao = new QLabel("Nenhuma sessão selecionada", this);
    lblInfoSessao->setWordWrap(true);
    lblInfoSessao->setStyleSheet(estiloCaixaInterna);

    listaItens = new QListWidget(this);
    // Seletor "QListWidget": sem ele a borda/padding também iam para a barra de rolagem
    // (a "caixa vazia" que surgia quando a lista enchia). A barra segue o estilo global.
    listaItens->setStyleSheet(
        "QListWidget { color: " COR_TEXTO "; font-size: 14px; background: transparent; "
        "padding: 8px; border: 1px solid " COR_BORDA "; border-radius: 3px; }"
        "QListWidget::item { padding: 4px 2px; }");
    listaItens->setSelectionMode(QAbstractItemView::NoSelection);
    listaItens->setFocusPolicy(Qt::NoFocus);

    lblVazio = new QLabel("Seu carrinho está vazio", this);
    lblVazio->setAlignment(Qt::AlignCenter);
    lblVazio->setWordWrap(true);
    lblVazio->setStyleSheet("color: " COR_DESATIVADO "; font-size: 14px; background: transparent; border: none;");

    lblCombosAplicados = new QLabel("", this);
    lblCombosAplicados->setWordWrap(true);
    lblCombosAplicados->hide(); // Só aparece quando houver combo aplicado
    lblCombosAplicados->setStyleSheet("color: " COR_SUCESSO "; font-weight: bold; font-size: 14px; background: transparent; padding: 16px; border: 1px solid " COR_BORDA "; border-radius: 3px;");

    lblTotal = new QLabel("Total: R$ 0,00", this);
    lblTotal->setStyleSheet("color: " COR_SUCESSO "; font-size: 18px; font-weight: bold; padding-top: 4px; background: transparent; border: none; ");
    lblTotal->setAlignment(Qt::AlignRight);

    layout->addWidget(lblTitulo);
    layout->addWidget(lblInfoSessao);
    layout->addWidget(listaItens, 1);
    layout->addWidget(lblVazio, 1);
    layout->addWidget(lblCombosAplicados);
    layout->addWidget(lblTotal);
}

/** @details Preenche lista com ingressos e bomboniere, calcula descontos de combos e formata o total. */
void SidebarCarrinhoWidget::atualizar(const Carrinho& carrinho, bool mostrarBomboniere) {
    listaItens->clear();

    if (Sessao* sessao = carrinho.getSessao()) {
        QString info = QString("Filme: %1\n%2")
                           .arg(QString::fromStdString(sessao->getFilme().getTitulo()))
                           .arg(QString::fromStdString(sessao->getDescricaoCompleta()));
        lblInfoSessao->setText(info);
    } else {
        lblInfoSessao->setText("Compra somente de bomboniere\n(sem sessão de filme)");
    }

    for (const auto& ing : carrinho.getIngressos()) {
        listaItens->addItem(QString("%1 - %2")
                                .arg(QString::fromStdString(ing->getDescricao()))
                                .arg(formatarMoeda(ing->calcularPreco())));
    }

    if (mostrarBomboniere) {
        for (const auto& item : carrinho.getItensBomboniere()) {
            listaItens->addItem(QString("%1 - %2")
                                    .arg(QString::fromStdString(item.getNome()))
                                    .arg(formatarMoeda(item.getPreco())));
        }
    }

    // Sem itens: em vez de uma caixa vazia com borda, mostra um aviso discreto.
    const bool semItens = listaItens->count() == 0;
    listaItens->setVisible(!semItens);
    lblVazio->setVisible(semItens);

    std::vector<std::string> combos;
    double desconto = carrinho.calcularDescontoCombos(combos);
    
    if (mostrarBomboniere && !combos.empty()) {
        QString txtCombos = "Combos aplicados:\n";
        for (const auto& c : combos) {
            txtCombos += QString("- %1\n").arg(QString::fromStdString(c));
        }
        txtCombos += QString("Desconto total: -%1").arg(formatarMoeda(desconto));
        lblCombosAplicados->setText(txtCombos);
        lblCombosAplicados->show();
    } else {
        lblCombosAplicados->hide();
    }

    double total = mostrarBomboniere ? carrinho.calcularTotalFinal() : carrinho.calcularSubtotalIngressos();
    lblTotal->setText(QString("Total: %1").arg(formatarMoeda(total)));
}

/** @details Alterna entre largura fixa (kLarguraPadrao) nas telas 2/3 e largura expansível na tela 4. */
void SidebarCarrinhoWidget::definirModoExpandido(bool expandido) {
    if (expandido) {
        setMinimumWidth(300);
        setMaximumWidth(QWIDGETSIZE_MAX);
        lblTotal->hide();
    } else {
        setFixedWidth(kLarguraPadrao); // Mesma largura nas telas 2 e 3
        lblTotal->show();
    }
}