#ifndef TELA3_BOMBONIERE_WIDGET_H
#define TELA3_BOMBONIERE_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <map>
#include <vector>
#include "Carrinho.h"
#include "RepositorioGenerico.h"
#include "ItemBomboniere.h"
#include "SidebarCarrinhoWidget.h"

class Tela3BomboniereWidget : public QWidget {
    Q_OBJECT

private:
    RepositorioGenerico<ItemBomboniere>& repoBomboniere;
    Carrinho& carrinho;

    QVBoxLayout* catalogoLayout;
    QScrollArea* scrollArea;
    QPushButton* btnVoltar;
    QPushButton* btnAvancar;
    SidebarCarrinhoWidget* sidebar;

    std::map<int, QLabel*> labelsQuantidade;

    void reconstruirCatalogo();
    void atualizarContadores();
    void atualizarResumo();      // contadores + sidebar + botoes de navegacao
    void atualizarNavegacao();   // texto do Voltar e habilitacao do Pagamento
    void showEvent(QShowEvent* event) override;

public:
    Tela3BomboniereWidget(RepositorioGenerico<ItemBomboniere>& bomboniere,
                          Carrinho& carrinho,
                          QWidget* parent = nullptr);

    void atualizarTela();

signals:
    void voltarParaAssentos();
    void avancarParaPagamento();
    void voltarParaInicio();

private slots:
    void onVoltarClicado();
    void onAdicionarItem(int idProduto);
    void onRemoverItem(int idProduto);
    void onAdicionarCombo(const std::vector<int>& idsProdutos);
};

#endif // TELA3_BOMBONIERE_WIDGET_H