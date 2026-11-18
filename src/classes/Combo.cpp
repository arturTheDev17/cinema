/**
 * @file Combo.cpp
 * @brief Implementação da classe Combo e validação de suas regras promocionais.
 */

#include "Combo.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>

int Combo::proximoId = 0;

/** @details Decrementa proximoId para gerar ID negativo e valida lista de produtos não vazia e desconto positivo. */
Combo::Combo(const std::string& nome, const std::vector<int>& idsProdutos, double desconto)
    : Produto(--proximoId, nome), idsProdutos(idsProdutos), desconto(desconto) {
    if (idsProdutos.empty()) throw std::invalid_argument("Combo sem produtos.");
    if (!(desconto > 0.0)) throw std::invalid_argument("Desconto do combo deve ser maior que zero.");
}

const std::vector<int>& Combo::getIdsProdutos() const { return idsProdutos; }
double Combo::getDesconto() const { return desconto; }
double Combo::getValorNoCarrinho() const { return -desconto; }

/** @details Formata o nome do combo acompanhado do valor de desconto com duas casas decimais. */
std::string Combo::getDescricao() const {
    std::ostringstream os;
    os << getNome() << " (desconto R$ " << std::fixed << std::setprecision(2) << desconto << ")";
    return os.str();
}