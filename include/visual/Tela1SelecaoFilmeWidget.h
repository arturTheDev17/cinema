/**
 * @file Tela1SelecaoFilmeWidget.h
 * @brief Declaração da Tela 1: escolha do filme, da sessão e da quantidade de ingressos.
 */

#ifndef TELA1_SELECAO_FILME_WIDGET_H
#define TELA1_SELECAO_FILME_WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QResizeEvent>
#include <QShowEvent>
#include <QEvent>
#include <vector>
#include "RepositorioGenerico.h"
#include "Filme.h"
#include "Sessao.h"
#include "Carrinho.h"

/**
 * @class Tela1SelecaoFilmeWidget
 * @brief Tela 1: escolha do filme, da sessão e da quantidade de ingressos (inteira/meia).
 */
class Tela1SelecaoFilmeWidget : public QWidget {
    Q_OBJECT

private:
    RepositorioGenerico<Filme>& repoFilmes;   ///< Referência ao repositório de filmes.
    RepositorioGenerico<Sessao>& repoSessoes; ///< Referência ao repositório de sessões.
    Carrinho& carrinho;                       ///< Referência ao carrinho compartilhado.

    QScrollArea* scrollFilmes;                ///< Área rolável vertical para os cards de filmes.
    QWidget* containerFilmesCards;            ///< Contêiner da grade responsiva de cards.
    QListWidget* listaSessoes;                ///< Lista das sessões disponíveis para o filme escolhido.
    QLabel* lblDetalhesFilme;                 ///< Rótulo com detalhes do filme selecionado.
    QPushButton* btnProsseguir;               ///< Botão para avançar à escolha de assentos.
    QPushButton* btnVoltar;                   ///< Botão para retornar à tela inicial.

    QPushButton* btnMaisInteira;              ///< Incrementa quantidade de ingressos inteiros.
    QPushButton* btnMenosInteira;             ///< Decrementa quantidade de ingressos inteiros.
    QLabel* lblQtdInteira;                    ///< Exibe quantidade de ingressos inteiros.
    int qtdInteiraVal;                        ///< Quantidade numérica de ingressos inteiros.

    QPushButton* btnMaisMeia;                 ///< Incrementa quantidade de ingressos meia-entrada.
    QPushButton* btnMenosMeia;                ///< Decrementa quantidade de ingressos meia-entrada.
    QLabel* lblQtdMeia;                       ///< Exibe quantidade de ingressos meia-entrada.
    int qtdMeiaVal;                           ///< Quantidade numérica de ingressos meia-entrada.

    int filmeSelecionadoIndex;                ///< Índice do filme atualmente selecionado (-1 se nenhum).
    static constexpr int kLarguraCard = 160;  ///< Largura fixa de cada card de filme em pixels.
    static constexpr int kAlturaCard = 230;   ///< Altura fixa de cada card de filme em pixels.
    int colunasAtuais;                        ///< Colunas da grade de cards na última reconstrução.
    std::vector<int> indicesSessoesFiltradas; ///< Índices reais em repoSessoes correspondentes às linhas da lista.

    /** @brief Calcula quantas colunas de cards cabem na largura útil do viewport. @return Número de colunas (>= 1). */
    int calcularColunas() const;

    /** @brief Reconstrói a grade de cards de filmes adaptando-a à largura atual. */
    void reconstruirCardsFilmes();

    /** @brief Atualiza a lista de sessões filtradas para o filme selecionado. */
    void reconstruirListaSessoes();

protected:
    /** @brief Reorganiza os cards quando a janela é redimensionada e muda o número de colunas. @param[in] event Evento Qt. */
    void resizeEvent(QResizeEvent* event) override;

    /** @brief Reorganiza os cards ao exibir a tela caso a quantidade de colunas tenha mudado. @param[in] event Evento Qt. */
    void showEvent(QShowEvent* event) override;

    /** @brief Observa eventos de resize no viewport de scrollFilmes para atualizar a grade. @param[in] alvo Objeto observado. @param[in] evento Evento Qt. @return Resultado de QWidget::eventFilter(). */
    bool eventFilter(QObject* alvo, QEvent* evento) override;

public:
    /**
     * @brief Constrói a tela de seleção de filmes e sessões.
     * @param[in] filmes Repositório de filmes em cartaz.
     * @param[in] sessoes Repositório de sessões disponíveis.
     * @param[in] carrinho Carrinho compartilhado da compra.
     * @param[in] parent Widget pai no Qt (opcional).
     */
    Tela1SelecaoFilmeWidget(RepositorioGenerico<Filme>& filmes,
                            RepositorioGenerico<Sessao>& sessoes,
                            Carrinho& carrinho,
                            QWidget* parent = nullptr);

    /** @brief Restaura a tela ao estado inicial (sem filme, sessão ou ingressos). */
    void resetar();

    /** @brief Atualiza cards e sessões após novos cadastros pelo AdminConsole. */
    void atualizarCatalogo();

signals:
    /** @brief Emitido quando sessão e ingressos são validados e adicionados ao carrinho. */
    void avancarParaAssentos();

    /** @brief Emitido ao clicar em Voltar para retornar à tela inicial. */
    void voltarParaInicio();

private slots:
    /** @brief Seleciona o filme clicado, mostra seus detalhes e lista suas sessões. @param[in] indexFilme Índice em repoFilmes. */
    void onFilmeCardClicado(int indexFilme);

    /** @brief Atualiza validação do avanço quando uma sessão é selecionada. @param[in] row Linha selecionada. */
    void onSessaoSelecionada(int row);

    /** @brief Habilita btnProsseguir apenas se houver sessão válida e ao menos 1 ingresso. */
    void onQuantidadeAlterada();

    /** @brief Valida assentos livres, preenche o carrinho com os ingressos e emite avancarParaAssentos(). */
    void onProsseguirClicado();
};

#endif