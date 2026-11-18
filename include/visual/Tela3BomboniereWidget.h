/**
 * @file Tela3BomboniereWidget.h
 * @brief Declaração da Tela 3: bomboniere com produtos avulsos e combos gerados do Carrinho.
 */

#ifndef TELA3_BOMBONIERE_WIDGET_H
#define TELA3_BOMBONIERE_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QShowEvent>
#include <QVBoxLayout>
#include <map>
#include <vector>
#include "Carrinho.h"
#include "RepositorioGenerico.h"
#include "ItemBomboniere.h"
#include "SidebarCarrinhoWidget.h"

/**
 * @class Tela3BomboniereWidget
 * @brief Tela 3: catálogo de bomboniere com produtos avulsos e botões de combos gerados das regras do Carrinho.
 */
class Tela3BomboniereWidget : public QWidget {
    Q_OBJECT

private:
    RepositorioGenerico<ItemBomboniere>& repoBomboniere; ///< Repositório de produtos da bomboniere.
    Carrinho& carrinho;                                  ///< Referência ao carrinho da compra.

    QVBoxLayout* catalogoLayout;                         ///< Layout principal da área do catálogo.
    size_t itensRenderizados = static_cast<size_t>(-1);  ///< Tamanho do catálogo na última reconstrução.
    QHBoxLayout* combosLayout;                           ///< Layout horizontal para os botões de combos.
    QScrollArea* scrollArea;                             ///< Área rolável dos cards de produtos.
    QPushButton* btnVoltar;                              ///< Botão de voltar (Assentos ou Início).
    QPushButton* btnAvancar;                             ///< Botão para ir ao pagamento.
    SidebarCarrinhoWidget* sidebar;                      ///< Painel lateral de resumo do pedido.

    std::map<int, QLabel*> labelsQuantidade;             ///< Mapeia ID do produto ao rótulo de quantidade no card.

    /** @brief Gera os botões de atalho de combos com base nas regras cadastradas no Carrinho. */
    void reconstruirCombos();

    /** @brief Obtém o nome de um produto pelo ID no repositório. @param[in] idProduto ID procurado. @return Nome ou vazio. */
    QString nomeProduto(int idProduto) const;

    /** @brief Reconstrói todos os cards visuais do catálogo a partir de repoBomboniere. */
    void reconstruirCatalogo();

    /** @brief Atualiza os contadores numéricos exibidos em cada card. */
    void atualizarContadores();

    /** @brief Atualiza contadores, sidebar do carrinho e estado dos botões de navegação. */
    void atualizarResumo();

    /** @brief Ajusta o texto de Voltar e habilita o botão de pagamento se o carrinho não estiver vazio. */
    void atualizarNavegacao();

protected:
    /** @brief Sincroniza o resumo sempre que a tela é exibida. @param[in] event Evento Qt. */
    void showEvent(QShowEvent* event) override;

public:
    /**
     * @brief Constrói a tela da bomboniere.
     * @param[in] bomboniere Repositório de itens da bomboniere.
     * @param[in] carrinho Carrinho da compra atual.
     * @param[in] parent Widget pai no Qt (opcional).
     */
    Tela3BomboniereWidget(RepositorioGenerico<ItemBomboniere>& bomboniere,
                          Carrinho& carrinho,
                          QWidget* parent = nullptr);

    /** @brief Atualiza a tela reconstruindo combos e cards caso o tamanho do catálogo tenha mudado. */
    void atualizarTela();

    /** @brief Força a reconstrução completa do catálogo e combos (após adição, alteração ou remoção pelo admin). */
    void atualizarCatalogo();

signals:
    /** @brief Emitido quando o usuário retorna para a tela de assentos. */
    void voltarParaAssentos();

    /** @brief Emitido quando o usuário avança para a tela de pagamento. */
    void avancarParaPagamento();

    /** @brief Emitido quando o usuário volta ao início no fluxo somente bomboniere. */
    void voltarParaInicio();

private slots:
    /** @brief Emite voltarParaAssentos() se houver sessão no carrinho ou voltarParaInicio() caso contrário. */
    void onVoltarClicado();

    /** @brief Adiciona uma unidade do produto ao carrinho. @param[in] idProduto ID do produto. */
    void onAdicionarItem(int idProduto);

    /** @brief Remove uma unidade do produto do carrinho. @param[in] idProduto ID do produto. */
    void onRemoverItem(int idProduto);

    /** @brief Adiciona ao carrinho todos os produtos integrantes de um combo. @param[in] idsProdutos Vetor de IDs. */
    void onAdicionarCombo(const std::vector<int>& idsProdutos);
};

#endif // TELA3_BOMBONIERE_WIDGET_H