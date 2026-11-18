/**
 * @file Tela4PagamentoWidget.h
 * @brief Declaração da Tela 4: resumo final e pagamento simulado.
 */

#ifndef TELA4_PAGAMENTO_WIDGET_H
#define TELA4_PAGAMENTO_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QTimer>
#include <QComboBox>
#include <QLabel>
#include "Carrinho.h"
#include "SidebarCarrinhoWidget.h"

/**
 * @class Tela4PagamentoWidget
 * @brief Tela 4: revisão expandida do pedido e pagamento simulado (segurar o botão por 2 segundos).
 */
class Tela4PagamentoWidget : public QWidget {
    Q_OBJECT

private:
    Carrinho& carrinho;              ///< Referência ao carrinho a ser finalizado.
    SidebarCarrinhoWidget* sidebar;  ///< Painel de resumo em modo expandido.
    QComboBox* comboFormaPagamento;  ///< Seletor de meio de pagamento (Crédito, Débito, PIX).
    QLabel* lblValorDestaque;        ///< Destaque do valor total a pagar.
    QPushButton* btnVoltar;          ///< Botão para retornar à bomboniere.
    QPushButton* btnPagar;           ///< Botão de confirmação por pressão contínua de 2 segundos.
    QProgressBar* barraProgresso;    ///< Barra de progresso da confirmação (0 a 2000 ms).
    QTimer* timerPagamento;          ///< Temporizador de 50 ms que incrementa progressoMs.
    int progressoMs;                 ///< Tempo acumulado com o botão pressionado em milissegundos.

public:
    /**
     * @brief Constrói a tela de pagamento.
     * @param[in] carrinho Carrinho da compra atual.
     * @param[in] parent Widget pai no Qt (opcional).
     */
    Tela4PagamentoWidget(Carrinho& carrinho, QWidget* parent = nullptr);

    /** @brief Zera o progresso, atualiza o resumo expandido, valida o botão e mostra o valor final. */
    void carregarResumo();

signals:
    /** @brief Emitido quando o usuário decide voltar à bomboniere. */
    void voltarParaBomboniere();

    /** @brief Emitido após a conclusão do pagamento e efetivação da compra. */
    void compraFinalizadaComSucesso();

private slots:
    /** @brief Inicia a contagem de 2 segundos ao pressionar btnPagar. */
    void onBotaoPagarPressionado();

    /** @brief Interrompe e zera o progresso caso o botão seja solto antes dos 2 segundos. */
    void onBotaoPagarSolto();

    /** @brief Avança o progresso em 50 ms e finaliza a compra ao atingir 2000 ms. */
    void onTimerTick();
};

#endif