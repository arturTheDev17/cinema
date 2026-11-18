/**
 * @file ItemBomboniere.cpp
 * @brief Implementação dos métodos e validações da classe ItemBomboniere.
 */

#include "ItemBomboniere.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>

int ItemBomboniere::proximoId = 1;

/** @details Consome proximoId e valida o invariante de preço > 0 antes de concluir a construção. */
ItemBomboniere::ItemBomboniere(const std::string& nome, double preco, const std::string& caminhoImagem)
    : Produto(proximoId++, nome), preco(preco), caminhoImagem(caminhoImagem) {
    if (!(preco > 0.0)) throw std::invalid_argument("Preco deve ser maior que zero."); // Igual a setPreco
}

/** @details Valida preço > 0 e ajusta proximoId para evitar colisões com novos cadastros automáticos. */
ItemBomboniere::ItemBomboniere(int idEspecifico, const std::string& nome, double preco, const std::string& caminhoImagem)
    : Produto(idEspecifico, nome), preco(preco), caminhoImagem(caminhoImagem) {
    if (!(preco > 0.0)) throw std::invalid_argument("Preco deve ser maior que zero.");
    if (idEspecifico >= proximoId) {
        proximoId = idEspecifico + 1;
    }
}

double ItemBomboniere::getPreco() const { return preco; }
double ItemBomboniere::getValorNoCarrinho() const { return preco; }

/** @details Valida e atualiza o preço unitário do item. */
void ItemBomboniere::setPreco(double novoPreco) {
    if (novoPreco <= 0.0) throw std::invalid_argument("Preco deve ser maior que zero.");
    preco = novoPreco;
}

void ItemBomboniere::setCaminhoImagem(const std::string& novoCaminho) { caminhoImagem = novoCaminho; }

/** @details Formata descrição com ID, nome e preço com duas casas decimais. */
std::string ItemBomboniere::getDescricao() const {
    std::ostringstream os;
    os << "[ID " << getId() << "] " << getNome() << " - R$ " << std::fixed << std::setprecision(2) << preco;
    return os.str();
}

const std::string& ItemBomboniere::getCaminhoImagem() const { return caminhoImagem; }

/** @details Compara dois itens exclusivamente pelo identificador herdado de Produto. */
bool ItemBomboniere::operator==(const ItemBomboniere& outro) const {
    return getId() == outro.getId();
}