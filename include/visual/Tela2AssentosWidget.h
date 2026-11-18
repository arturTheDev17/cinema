/**
 * @file Tela2AssentosWidget.h
 * @brief Declaração da Tela 2: mapa de assentos da sala.
 */

#ifndef TELA2_ASSENTOS_WIDGET_H
#define TELA2_ASSENTOS_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <vector>
#include "Carrinho.h"
#include "SidebarCarrinhoWidget.h"

/**
 * @class Tela2AssentosWidget
 * @brief Tela 2: mapa interativo de assentos da sala; exige um assento por ingresso.
 */
class Tela2AssentosWidget : public QWidget {
    Q_OBJECT

private:
    Carrinho& carrinho;                              ///< Referência ao carrinho da compra atual.
    QGridLayout* gridAssentos;                       ///< Grade visual dos assentos da sala.
    QLabel* lblInstrucao;                            ///< Contador de assentos escolhidos/necessários.
    QPushButton* btnVoltar;                          ///< Botão para voltar à seleção de filmes.
    QPushButton* btnAvancar;                         ///< Botão para avançar à bomboniere.
    SidebarCarrinhoWidget* sidebar;                  ///< Resumo lateral do pedido.

    std::vector<std::vector<QPushButton*>> botoesAssentos; ///< Matriz de botões espelhando a sala.
    std::vector<Assento*> assentosSelecionados;            ///< Assentos atualmente reservados nesta tela.

    /** @brief Reconstrói a matriz de botões com as cores de estado (LIVRE, RESERVADO, OCUPADO). */
    void reconstruirGrade();

    /** @brief Atualiza a instrução de contagem e habilita o botão de avanço ao completar a cota. */
    void atualizarEstadoBotoes();

public:
    /**
     * @brief Constrói a tela de seleção de assentos.
     * @param[in] carrinho Carrinho compartilhado da compra.
     * @param[in] parent Widget pai no Qt (opcional).
     */
    Tela2AssentosWidget(Carrinho& carrinho, QWidget* parent = nullptr);

    /**
     * @brief Prepara a tela para o carrinho atual.
     * @param[in] preservarSelecao false (padrão) começa do zero liberando assentos antigos;
     *                             true mantém os assentos já escolhidos (ao voltar da bomboniere).
     */
    void carregarSessaoAtual(bool preservarSelecao = false);

    /** @brief Devolve ao estado LIVRE os assentos escolhidos nesta tela (voltar, inatividade). */
    void liberarSelecao();

signals:
    /** @brief Emitido quando o usuário retorna à tela de seleção de filmes. */
    void voltarParaFilmes();

    /** @brief Emitido após vincular todos os assentos escolhidos aos ingressos do carrinho. */
    void avancarParaBomboniere();

private slots:
    /**
     * @brief Alterna a reserva do assento clicado (LIVRE <-> RESERVADO) respeitando a cota.
     * @param[in] fileira Fileira do assento (base 1).
     * @param[in] coluna Coluna do assento (base 1).
     */
    void onAssentoClicado(int fileira, int coluna);

    /** @brief Libera a seleção atual e emite voltarParaFilmes(). */
    void onVoltarClicado();

    /** @brief Confirma a vinculação dos assentos aos ingressos e emite avancarParaBomboniere(). */
    void onAvancarClicado();
};

#endif