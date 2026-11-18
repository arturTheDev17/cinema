/**
 * @file Produto.h
 * @brief Declaração da classe base abstrata Produto e de seu operador de inserção em stream.
 */

#ifndef PRODUTO_H
#define PRODUTO_H

#include <iosfwd>
#include <string>

/**
 * @class Produto
 * @brief Base abstrata polimórfica para tudo o que o carrinho vende (ItemBomboniere, Combo).
 * @note Construtor e atribuição de cópia são protegidos para evitar fatiamento (slicing) via Produto&.
 */
class Produto {
private:
    int id;           ///< Identificador numérico único do produto.
    std::string nome; ///< Nome comercial do produto (invariante: nunca vazio).

protected:
    /**
     * @brief Construtor protegido invocado pelas subclasses.
     * @param[in] id Identificador numérico do produto.
     * @param[in] nome Nome do produto (não pode ser vazio).
     * @throws std::invalid_argument Se nome for vazio.
     */
    Produto(int id, const std::string& nome);

    /** @brief Construtor de cópia protegido para evitar fatiamento polimórfico. */
    Produto(const Produto&) = default;

    /** @brief Operador de atribuição por cópia protegido para evitar fatiamento polimórfico. */
    Produto& operator=(const Produto&) = default;

public:
    /** @brief Destrutor virtual padrão para destruição polimórfica segura. */
    virtual ~Produto() = default;

    /** @brief Retorna o identificador único do produto. @return Inteiro identificador. */
    int getId() const;

    /** @brief Retorna o nome do produto. @return Referência constante para a string do nome. */
    const std::string& getNome() const;

    /**
     * @brief Valor que o produto contribui ao total do carrinho (com sinal: item > 0, combo < 0).
     * @return Contribuição financeira em reais (positiva ou negativa).
     * @note Contrato LSP: o contrato da base representa "quanto esta linha soma ao total do carrinho",
     *       e não "preço de prateleira". ItemBomboniere devolve o preço (> 0) e Combo devolve
     *       -desconto (< 0), mantendo total intercambialidade em Carrinho::calcularTotalFinal().
     */
    virtual double getValorNoCarrinho() const = 0;

    /**
     * @brief Altera o nome do produto mantendo o invariante de não vazio.
     * @param[in] novoNome Novo nome comercial do produto.
     * @throws std::invalid_argument Se novoNome for vazio.
     */
    void setNome(const std::string& novoNome);

    /**
     * @brief Gera uma descrição textual do produto.
     * @return String descritiva (por padrão, retorna o nome do produto).
     */
    virtual std::string getDescricao() const;
};

/**
 * @brief Sobrecarga do operador de inserção em stream utilizando despacho polimórfico.
 * @param[in,out] os Stream de saída.
 * @param[in] produto Referência polimórfica para Produto.
 * @return Referência para a stream de saída.
 */
std::ostream& operator<<(std::ostream& os, const Produto& produto);

#endif