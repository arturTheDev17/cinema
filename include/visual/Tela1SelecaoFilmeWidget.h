#ifndef TELA1_SELECAO_FILME_WIDGET_H
#define TELA1_SELECAO_FILME_WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QResizeEvent>
#include <QShowEvent>
#include <vector>
#include "RepositorioGenerico.h"
#include "Filme.h"
#include "Sessao.h"
#include "Carrinho.h"

class Tela1SelecaoFilmeWidget : public QWidget {
    Q_OBJECT

private:
    RepositorioGenerico<Filme>& repoFilmes;
    RepositorioGenerico<Sessao>& repoSessoes;
    Carrinho& carrinho;

    QWidget* containerFilmesCards;
    QListWidget* listaSessoes;
    QLabel* lblDetalhesFilme;
    QPushButton* btnProsseguir;
    QPushButton* btnVoltar;

    // Controles de quantidade estilo bomboniere (Inteira)
    QPushButton* btnMaisInteira;
    QPushButton* btnMenosInteira;
    QLabel* lblQtdInteira;
    int qtdInteiraVal;

    // Controles de quantidade estilo bomboniere (Meia-Entrada)
    QPushButton* btnMaisMeia;
    QPushButton* btnMenosMeia;
    QLabel* lblQtdMeia;
    int qtdMeiaVal;

    int filmeSelecionadoIndex;
    std::vector<int> indicesSessoesFiltradas;

    void reconstruirCardsFilmes();
    void reconstruirListaSessoes();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

public:
    Tela1SelecaoFilmeWidget(RepositorioGenerico<Filme>& filmes,
                            RepositorioGenerico<Sessao>& sessoes,
                            Carrinho& carrinho,
                            QWidget* parent = nullptr);
    void resetar();
    void atualizarCatalogo(); // chamar apos filmes/sessoes serem adicionados

signals:
    void avancarParaAssentos();
    void voltarParaInicio();

private slots:
    void onFilmeCardClicado(int indexFilme);
    void onSessaoSelecionada(int row);
    void onQuantidadeAlterada();
    void onProsseguirClicado();
};

#endif