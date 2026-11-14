#ifndef SIDEBAR_CARRINHO_WIDGET_H
#define SIDEBAR_CARRINHO_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include "Carrinho.h"

class SidebarCarrinhoWidget : public QWidget {
    Q_OBJECT

private:
    QLabel* lblTitulo;
    QLabel* lblInfoSessao;
    QListWidget* listaItens;
    QLabel* lblCombosAplicados;
    QLabel* lblTotal;

public:
    explicit SidebarCarrinhoWidget(QWidget* parent = nullptr);
    void atualizar(const Carrinho& carrinho, bool mostrarBomboniere = true);
    void definirModoExpandido(bool expandido);
};

#endif
