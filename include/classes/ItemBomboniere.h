/**
 * @file ItemBomboniere.h
 * @brief Declaração da classe ItemBomboniere, derivada de Produto.
 */

#ifndef ITEM_BOMBONIERE_H
#define ITEM_BOMBONIERE_H

#include <string>
#include "Produto.h"

/**
 * @class ItemBomboniere
 * @brief Produto avulso da bomboniere com preço positivo, imagem opcional e sobrecarga de construtores.
 */
class ItemBomboniere : public Produto {
private:
    double preco;              ///< Preço unitário de prateleira em reais (invariante: > 0).
    std::string caminhoImagem; ///< Caminho do recurso gráfico da miniatura do produto.

    static int proximoId;      ///< Contador estático positivo para geração automática de IDs.

public:
    /**
     * @brief Constrói um item atribuindo automaticamente o próximo ID disponível.
     * @param[in] nome Nome do produto (não vazio).
     * @param[in] preco Preço unitário em reais (deve ser > 0).
     * @param[in] caminhoImagem Caminho opcional da imagem do produto.
     * @throws std::invalid_argument Se nome for vazio ou preco <= 0.
     */
    ItemBomboniere(const std::string& nome, double preco, const std::string& caminhoImagem = "");

    /**
     * @brief Constrói um item com ID explícito (sobrecarga para carga de catálogo ou arquivo).
     * @param[in] idEspecifico Identificador fixo desejado.
     * @param[in] nome Nome do produto (não vazio).
     * @param[in] preco Preço unitário em reais (deve ser > 0).
     * @param[in] caminhoImagem Caminho opcional da imagem do produto.
     * @throws std::invalid_argument Se nome for vazio ou preco <= 0.
     * @note Atualiza proximoId caso idEspecifico seja maior ou igual ao contador atual.
     */
    ItemBomboniere(int idEspecifico, const std::string& nome, double preco, const std::string& caminhoImagem = "");

    /** @brief Retorna o preço de prateleira do item (> 0). @return Valor unitário em reais. */
    double getPreco() const;

    /** @brief Retorna a contribuição positiva do item no carrinho (igual a getPreco()). @return Valor > 0 em reais. */
    double getValorNoCarrinho() const override;

    /**
     * @brief Atualiza o preço unitário do produto.
     * @param[in] novoPreco Novo valor em reais (deve ser > 0).
     * @throws std::invalid_argument Se novoPreco <= 0.
     */
    void setPreco(double novoPreco);

    /**
     * @brief Atualiza o caminho da imagem associada ao produto.
     * @param[in] novoCaminho Novo caminho de arquivo ou recurso Qt.
     */
    void setCaminhoImagem(const std::string& novoCaminho);

    /**
     * @brief Formata a descrição do item com ID, nome e preço.
     * @return String formatada (ex.: "[ID 1] Pipoca Grande - R$ 26.00").
     */
    std::string getDescricao() const override;

    /** @brief Retorna o caminho da imagem do produto. @return Referência constante para a string do caminho. */
    const std::string& getCaminhoImagem() const;

    /**
     * @brief Sobrecarga do operador de igualdade comparando dois itens pelo identificador.
     * @param[in] outro Outro ItemBomboniere a ser comparado.
     * @return true se ambos possuírem o mesmo ID; false caso contrário.
     * @note Disponível para algoritmos da STL (std::find, std::count).
     */
    bool operator==(const ItemBomboniere& outro) const;
};

#endif