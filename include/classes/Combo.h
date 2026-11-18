/**
 * @file Combo.h
 * @brief Declaração da classe Combo, derivada de Produto, para regras promocionais de desconto.
 */

#ifndef COMBO_H
#define COMBO_H

#include <string>
#include <vector>
#include "Produto.h"

/**
 * @class Combo
 * @brief Modela um combo promocional formado por um conjunto de produtos (por ID) que gera desconto.
 *
 * @note Justificativa LSP da herança: Produto modela "uma linha do carrinho", não "um item de estoque".
 *       Nesse sentido, o Combo é um Produto legítimo pois contribui com um valor ao total (-desconto).
 *       Limitação conhecida: não é vendido avulso, não entra em RepositorioGenerico<ItemBomboniere>
 *       e usa IDs negativos para nunca colidir com os de ItemBomboniere.
 */
class Combo : public Produto {
private:
    std::vector<int> idsProdutos; ///< IDs dos produtos necessários para formar o combo (permite repetições).
    double desconto;              ///< Valor absoluto do desconto em reais (invariante: > 0).
    static int proximoId;         ///< Contador estático negativo para evitar colisão de IDs com ItemBomboniere.

public:
    /**
     * @brief Constrói uma regra de combo promocional validando nome, itens e desconto.
     * @param[in] nome Nome do combo (não vazio).
     * @param[in] idsProdutos Lista de IDs de produtos exigidos (não pode ser vazia).
     * @param[in] desconto Valor do desconto em reais (deve ser > 0).
     * @throws std::invalid_argument Se nome ou idsProdutos forem vazios, ou se desconto <= 0.
     */
    Combo(const std::string& nome, const std::vector<int>& idsProdutos, double desconto);

    /** @brief Obtém os IDs dos produtos componentes do combo. @return Referência constante para o vetor de IDs. */
    const std::vector<int>& getIdsProdutos() const;

    /** @brief Obtém o valor absoluto do desconto em reais. @return Valor positivo do desconto. */
    double getDesconto() const;

    /** @brief Retorna a contribuição negativa do combo no carrinho (-desconto). @return Valor < 0 em reais. */
    double getValorNoCarrinho() const override;

    /**
     * @brief Gera descrição textual exibindo nome do combo e valor do desconto.
     * @return String formatada (ex.: "Combo Solo (desconto R$ 4.00)").
     */
    std::string getDescricao() const override;
};

#endif