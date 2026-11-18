/**
 * @file SidebarCarrinhoWidget.h
 * @brief Declaração do componente visual SidebarCarrinhoWidget para exibição do resumo do pedido.
 */

#ifndef SIDEBAR_CARRINHO_WIDGET_H
#define SIDEBAR_CARRINHO_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include "Carrinho.h"

/**
 * @class SidebarCarrinhoWidget
 * @brief Painel lateral com o resumo do pedido (sessão, itens, combos aplicados e total).
 */
class SidebarCarrinhoWidget : public QWidget {
    Q_OBJECT

private:
    static constexpr int kLarguraPadrao = 360; ///< Largura fixa utilizada nas telas 2 e 3.

    QLabel* lblTitulo;                         ///< Título do painel ("RESUMO DO PEDIDO").
    QLabel* lblInfoSessao;                     ///< Dados da sessão ou aviso de compra somente de bomboniere.
    QListWidget* listaItens;                   ///< Lista visual dos ingressos e produtos do pedido.
    QLabel* lblVazio;                          ///< Aviso exibido no lugar da lista quando não há itens.
    QLabel* lblCombosAplicados;                ///< Exibição dos combos formados e desconto total.
    QLabel* lblTotal;                          ///< Valor total do pedido.

public:
    /**
     * @brief Constrói o painel lateral de resumo do carrinho.
     * @param[in] parent Widget pai no Qt (opcional).
     */
    explicit SidebarCarrinhoWidget(QWidget* parent = nullptr);

    /**
     * @brief Atualiza todos os elementos visuais do resumo a partir do carrinho.
     * @param[in] carrinho Referência constante para o Carrinho atual.
     * @param[in] mostrarBomboniere true para incluir bomboniere e combos; false para mostrar apenas ingressos.
     */
    void atualizar(const Carrinho& carrinho, bool mostrarBomboniere = true);

    /**
     * @brief Alterna entre largura fixa lateral e modo expandido (usado na tela de pagamento).
     * @param[in] expandido true para expandir largura e ocultar total inferior; false para largura fixa.
     */
    void definirModoExpandido(bool expandido);
};

#endif