#include "SidebarCarrinhoWidget.h"
#include <QFont>
#include <QVBoxLayout>

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
    lblTitulo->setStyleSheet("color: #dec402; background: transparent; border: none;");

    QString estiloCaixaInterna = "color: #fbfef9; font-size: 14px; background: transparent; padding: 16px; border: 1px solid #3d4d52; border-radius: 3px;";

    lblInfoSessao = new QLabel("Nenhuma sessao selecionada", this);
    lblInfoSessao->setWordWrap(true);
    lblInfoSessao->setStyleSheet(estiloCaixaInterna);

    listaItens = new QListWidget(this);
    listaItens->setStyleSheet(estiloCaixaInterna);

    lblCombosAplicados = new QLabel("", this);
    lblCombosAplicados->setWordWrap(true);
    lblCombosAplicados->setStyleSheet("color: #7FBB3A; font-weight: bold; font-size: 14px; background: transparent; border: none; padding: 16px; border: 1px solid #3d4d52; border-radius: 3px;");

    lblTotal = new QLabel("Total: R$ 0,00", this);
    lblTotal->setStyleSheet("color: #7FBB3A; font-size: 18px; font-weight: bold; padding-top: 4px; background: transparent; border: none; ");
    lblTotal->setAlignment(Qt::AlignRight);

    layout->addWidget(lblTitulo);
    layout->addWidget(lblInfoSessao);
    layout->addWidget(listaItens, 1);
    layout->addWidget(lblCombosAplicados);
    layout->addWidget(lblTotal);
}

void SidebarCarrinhoWidget::atualizar(const Carrinho& carrinho, bool mostrarBomboniere) {
    listaItens->clear();

    if (Sessao* sessao = carrinho.getSessao()) {
        QString info = QString("Filme: %1\n%2")
                           .arg(QString::fromStdString(sessao->getFilme().getTitulo()))
                           .arg(QString::fromStdString(sessao->getDescricaoCompleta()));
        lblInfoSessao->setText(info);
    } else {
        lblInfoSessao->setText("Compra somente de bomboniere\n(sem sessao de filme)");
    }

    for (const auto& ing : carrinho.getIngressos()) {
        listaItens->addItem(QString("%1 - R$ %2")
                                .arg(QString::fromStdString(ing->getDescricao()))
                                .arg(ing->calcularPreco(), 0, 'f', 2));
    }

    if (mostrarBomboniere) {
        for (const auto& item : carrinho.getItensBomboniere()) {
            listaItens->addItem(QString("%1 - R$ %2")
                                    .arg(QString::fromStdString(item.getNome()))
                                    .arg(item.getPreco(), 0, 'f', 2));
        }
    }

    std::vector<std::string> combos;
    double desconto = carrinho.calcularDescontoCombos(combos);
    
    if (mostrarBomboniere && !combos.empty()) {
        QString txtCombos = "Combos aplicados:\n";
        for (const auto& c : combos) {
            txtCombos += QString("- %1\n").arg(QString::fromStdString(c));
        }
        txtCombos += QString("Desconto total: -R$ %1").arg(desconto, 0, 'f', 2);
        lblCombosAplicados->setText(txtCombos);
        lblCombosAplicados->show();
    } else {
        lblCombosAplicados->hide();
    }

    double total = mostrarBomboniere ? carrinho.calcularTotalFinal() : carrinho.calcularSubtotalIngressos();
    lblTotal->setText(QString("Total: R$ %1").arg(total, 0, 'f', 2));
}

void SidebarCarrinhoWidget::definirModoExpandido(bool expandido) {
    if (expandido) {
        setMaximumWidth(QWIDGETSIZE_MAX);
        lblTotal->hide();
    } else {
        setMaximumWidth(365);
        lblTotal->show();
    }
}