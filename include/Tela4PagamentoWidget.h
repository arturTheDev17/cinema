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

class Tela4PagamentoWidget : public QWidget {
    Q_OBJECT

private:
    Carrinho& carrinho;
    SidebarCarrinhoWidget* sidebar;
    QComboBox* comboFormaPagamento;
    QLabel* lblValorDestaque;
    QPushButton* btnVoltar;
    QPushButton* btnPagar;
    QProgressBar* barraProgresso;
    QTimer* timerPagamento;
    int progressoMs;

public:
    Tela4PagamentoWidget(Carrinho& carrinho, QWidget* parent = nullptr);
    void carregarResumo();

signals:
    void voltarParaBomboniere();
    void compraFinalizadaComSucesso();

private slots:
    void onBotaoPagarPressionado();
    void onBotaoPagarSolto();
    void onTimerTick();
};

#endif
