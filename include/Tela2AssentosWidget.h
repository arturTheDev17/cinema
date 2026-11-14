#ifndef TELA2_ASSENTOS_WIDGET_H
#define TELA2_ASSENTOS_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <vector>
#include "Carrinho.h"
#include "SidebarCarrinhoWidget.h"

class Tela2AssentosWidget : public QWidget {
    Q_OBJECT

private:
    Carrinho& carrinho;
    QGridLayout* gridAssentos;
    QLabel* lblInstrucao;
    QPushButton* btnVoltar;
    QPushButton* btnAvancar;
    SidebarCarrinhoWidget* sidebar;

    std::vector<std::vector<QPushButton*>> botoesAssentos;
    std::vector<Assento*> assentosSelecionados;

    void reconstruirGrade();
    void atualizarEstadoBotoes();

public:
    Tela2AssentosWidget(Carrinho& carrinho, QWidget* parent = nullptr);
    void carregarSessaoAtual();

signals:
    void voltarParaFilmes();
    void avancarParaBomboniere();

private slots:
    void onAssentoClicado(int fileira, int coluna);
    void onVoltarClicado();
    void onAvancarClicado();
};

#endif
