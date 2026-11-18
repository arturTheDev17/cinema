/**
 * @file MainWindow.h
 * @brief Declaração da janela principal MainWindow, orquestradora das telas, dados e inatividade.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include "RepositorioGenerico.h"
#include "Filme.h"
#include "Sessao.h"
#include "ItemBomboniere.h"
#include "Carrinho.h"
#include "AdminConsole.h"

class QEvent;
class QResizeEvent;
class QSizeGrip;
class BarraTituloWidget;
class TelaInicialWidget;
class Tela1SelecaoFilmeWidget;
class Tela2AssentosWidget;
class Tela3BomboniereWidget;
class Tela4PagamentoWidget;

/**
 * @class MainWindow
 * @brief Janela principal do totem: dona dos repositórios, do carrinho, das telas e da navegação.
 *
 * Monitora a inatividade do usuário; se não houver interação dentro do tempo limite,
 * cancela a compra em andamento (liberando assentos reservados) e retorna à tela inicial.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    static constexpr int kInatividadeMs = 300000; ///< Tempo limite de inatividade em milissegundos (5 minutos).

    // Ordem importa: o console de admin (declarado depois) é destruído antes dos repositórios.
    RepositorioGenerico<Filme> repoFilmes;              ///< Repositório central de filmes.
    RepositorioGenerico<Sessao> repoSessoes;            ///< Repositório central de sessões.
    RepositorioGenerico<ItemBomboniere> repoBomboniere; ///< Repositório central da bomboniere.
    Carrinho carrinho;                                  ///< Carrinho único compartilhado por referência.
    std::unique_ptr<AdminConsole> adminConsole;         ///< Controlador do console administrativo paralelo.

    QStackedWidget* stackedWidget;          ///< Gerenciador de pilha para troca entre as telas.
    TelaInicialWidget* telaInicial;         ///< Tela de boas-vindas e escolha do fluxo.
    Tela1SelecaoFilmeWidget* tela1;         ///< Tela 1: filmes, sessões e ingressos.
    Tela2AssentosWidget* tela2;             ///< Tela 2: mapa de assentos.
    Tela3BomboniereWidget* tela3;           ///< Tela 3: bomboniere e combos.
    Tela4PagamentoWidget* tela4;            ///< Tela 4: resumo e pagamento.
    QTimer* timerInatividade;               ///< Temporizador single-shot para cancelamento por ociosidade.
    BarraTituloWidget* barraTitulo;         ///< Barra de título customizada sem moldura nativa.
    QSizeGrip* alcaRedimensionar;           ///< Alça de redimensionamento no canto inferior direito.

    /** @brief Aplica a folha de estilo global (QSS) em tema escuro para toda a janela. */
    void aplicarEstiloGlobal();

    /** @brief Cancela a compra em andamento por inatividade, liberando assentos e voltando ao início. */
    void cancelarCompraPorInatividade();

    /** @brief Alterna entre janela maximizada, restaurada ou sai de tela cheia. */
    void alternarMaximizar();

    /** @brief Alterna o modo tela cheia (F11), ocultando barra de título e alça no modo totem. */
    void alternarTelaCheia();

protected:
    /**
     * @brief Intercepta cliques, toques e teclas na aplicação para reiniciar o timer de inatividade.
     * @param[in] alvo Objeto que recebeu o evento.
     * @param[in] evento Evento interceptado.
     * @return Resultado padrão de QMainWindow::eventFilter().
     */
    bool eventFilter(QObject* alvo, QEvent* evento) override;

    /**
     * @brief Reposiciona a alça de redimensionamento no canto inferior direito ao redimensionar.
     * @param[in] evento Evento de redimensionamento Qt.
     */
    void resizeEvent(QResizeEvent* evento) override;

public:
    /**
     * @brief Constrói a janela principal, inicializa dados, conecta sinais das telas e inicia o AdminConsole.
     * @param[in] parent Widget pai no Qt (opcional).
     */
    explicit MainWindow(QWidget* parent = nullptr);
};

#endif