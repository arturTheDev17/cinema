#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "RepositorioGenerico.h"
#include "Filme.h"
#include "Sessao.h"
#include "ItemBomboniere.h"
#include "Carrinho.h"

class TelaInicialWidget;
class Tela1SelecaoFilmeWidget;
class Tela2AssentosWidget;
class Tela3BomboniereWidget;
class Tela4PagamentoWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    RepositorioGenerico<Filme> repoFilmes;
    RepositorioGenerico<Sessao> repoSessoes;
    RepositorioGenerico<ItemBomboniere> repoBomboniere;
    Carrinho carrinho;

    QStackedWidget* stackedWidget;
    TelaInicialWidget* telaInicial;
    Tela1SelecaoFilmeWidget* tela1;
    Tela2AssentosWidget* tela2;
    Tela3BomboniereWidget* tela3;
    Tela4PagamentoWidget* tela4;

    void inicializarDados();
    void iniciarModoAdminConsole();
    void aplicarEstiloGlobal();
public:
    explicit MainWindow(QWidget* parent = nullptr);
};

#endif
