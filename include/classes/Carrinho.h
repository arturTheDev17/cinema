/**
 * @file Carrinho.h
 * @brief Declaração da classe Carrinho, gerenciadora do pedido ativo do totem.
 */

#ifndef CARRINHO_H
#define CARRINHO_H

#include "Sessao.h"
#include "Ingresso.h"
#include "ItemBomboniere.h"
#include "Combo.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @class Carrinho
 * @brief Gerencia sessão selecionada, ingressos polimórficos, itens de bomboniere e combos promocionais.
 *
 * @note Regra dos Três/Cinco: possui ingressos via std::unique_ptr (posse exclusiva), portanto
 *       cópia e atribuição por cópia são explicitamente proibidas (= delete). Vive como membro
 *       único da MainWindow e é compartilhado por referência.
 */
class Carrinho {
private:
    Sessao* sessaoSelecionada;                        ///< Ponteiro não-proprietário para a sessão escolhida.
    std::vector<std::unique_ptr<Ingresso>> ingressos; ///< Lista polimórfica de ingressos do pedido.
    std::vector<ItemBomboniere> itensBomboniere;      ///< Lista de produtos da bomboniere adicionados.
    std::vector<Combo> combosDisponiveis;             ///< Regras promocionais de combos cadastradas.

    /**
     * @brief Identifica os combos formados com os itens de bomboniere atuais.
     * @return Vetor de ponteiros não-proprietários para elementos de combosDisponiveis (sem cópias).
     */
    std::vector<const Combo*> combosAplicados() const;

public:
    /** @brief Constrói um carrinho vazio sem sessão selecionada. */
    Carrinho();

    /** @brief Construtor de cópia proibido (posse exclusiva de unique_ptr). */
    Carrinho(const Carrinho&) = delete;

    /** @brief Operador de atribuição por cópia proibido (posse exclusiva de unique_ptr). */
    Carrinho& operator=(const Carrinho&) = delete;

    /** @brief Destrutor padrão. */
    ~Carrinho() = default;

    /** @brief Define a sessão da compra atual. @param[in] sessao Ponteiro para a sessão escolhida. */
    void definirSessao(Sessao* sessao);

    /** @brief Obtém a sessão atual associada ao carrinho. @return Ponteiro para Sessao ou nullptr. */
    Sessao* getSessao() const;

    /**
     * @brief Adiciona um ingresso polimórfico ao carrinho transferindo sua posse.
     * @param[in] ingresso Ponteiro inteligente único para uma subclasse de Ingresso.
     */
    void adicionarIngresso(std::unique_ptr<Ingresso> ingresso);

    /** @brief Remove todos os ingressos do carrinho. */
    void limparIngressos();

    /** @brief Retorna os ingressos no carrinho. @return Referência constante ao vetor de unique_ptr. */
    const std::vector<std::unique_ptr<Ingresso>>& getIngressos() const;

    /** @brief Retorna a quantidade atual de ingressos. @return Número inteiro de ingressos. */
    int getQuantidadeIngressos() const;

    /**
     * @brief Vincula posicionalmente cada assento selecionado ao respectivo ingresso do carrinho.
     * @param[in] assentosSelecionados Vetor de ponteiros de Assento na ordem dos ingressos.
     * @return true se os tamanhos coincidirem; false caso contrário.
     */
    bool vincularAssentosAosIngressos(const std::vector<Assento*>& assentosSelecionados);

    /** @brief Registra uma regra promocional de combo no carrinho. @param[in] combo Objeto Combo. */
    void cadastrarCombo(const Combo& combo);

    /** @brief Lista os combos cadastrados. @return Referência constante para o vetor de Combo. */
    const std::vector<Combo>& getCombos() const;

    /** @brief Adiciona um item de bomboniere ao carrinho. @param[in] item Produto a adicionar. */
    void adicionarItemBomboniere(const ItemBomboniere& item);

    /**
     * @brief Sobrecarga do operador += equivalente a adicionarItemBomboniere().
     * @param[in] item Produto da bomboniere a adicionar.
     * @return Referência para o próprio Carrinho.
     */
    Carrinho& operator+=(const ItemBomboniere& item);

    /**
     * @brief Remove a primeira ocorrência do item de bomboniere com o ID informado.
     * @param[in] idProduto Identificador do produto a remover.
     */
    void removerItemBombonierePorId(int idProduto);

    /**
     * @brief Conta quantas unidades de um produto estão no carrinho.
     * @param[in] idProduto Identificador do produto.
     * @return Quantidade de unidades encontradas.
     */
    int contarQuantidadeProduto(int idProduto) const;

    /** @brief Retorna os itens de bomboniere no carrinho. @return Referência constante ao vetor. */
    const std::vector<ItemBomboniere>& getItensBomboniere() const;

    /**
     * @brief Verifica se não há ingressos nem itens de bomboniere no carrinho.
     * @return true quando não há nada a pagar; false caso contrário.
     */
    bool estaVazio() const;

    /** @brief Soma polimorficamente os preços finais de todos os ingressos. @return Subtotal em reais. */
    double calcularSubtotalIngressos() const;

    /**
     * @brief Soma os preços cheios dos itens avulsos da bomboniere, sem descontos de combo.
     * @return Subtotal bruto da bomboniere em reais (útil para relatórios/telas de valor cheio).
     */
    double calcularSubtotalBomboniere() const;

    /**
     * @brief Calcula o desconto acumulado pelos combos formados no pedido.
     * @param[out] combosAplicadosOut Vetor preenchido com os nomes dos combos aplicados.
     * @return Desconto total em reais.
     */
    double calcularDescontoCombos(std::vector<std::string>& combosAplicadosOut) const;

    /**
     * @brief Calcula o valor líquido final somando ingressos e getValorNoCarrinho() de itens e combos.
     * @return Total final em reais (garantido >= 0.0).
     */
    double calcularTotalFinal() const;

    /**
     * @brief Confirma definitivamente os assentos reservados na sessão atual e limpa o carrinho.
     */
    void finalizarCompra();

    /**
     * @brief Esvazia o carrinho e devolve ao estado LIVRE os assentos ainda RESERVADOS por ele.
     */
    void limparTudo();
};

#endif