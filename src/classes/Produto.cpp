/**
 * @file Produto.cpp
 * @brief Implementação da classe base abstrata Produto e de seu operador de inserção em stream.
 */

#include "Produto.h"
#include <ostream>
#include <stdexcept>

/** @details Valida o mesmo invariante de setNome() garantindo que nenhum Produto seja construído sem nome. */
Produto::Produto(int id, const std::string& nome) : id(id), nome(nome) {
    if (nome.empty()) throw std::invalid_argument("Nome vazio."); // Mesmo invariante de setNome()
}

int Produto::getId() const { return id; }
const std::string& Produto::getNome() const { return nome; }
std::string Produto::getDescricao() const { return nome; }

/** @details Atualiza o nome após garantir que novoNome não é vazio. */
void Produto::setNome(const std::string& novoNome) {
    if (novoNome.empty()) throw std::invalid_argument("Nome vazio.");
    nome = novoNome;
}

/** @details Invoca getDescricao() virtualmente para suportar impressão polimórfica em streams. */
std::ostream& operator<<(std::ostream& os, const Produto& produto) {
    return os << produto.getDescricao(); // Chamada virtual
}