/**
 * @file Ingresso.cpp
 * @brief Implementação da classe abstrata Ingresso e das subclasses IngressoInteira e IngressoMeia.
 */

#include "Ingresso.h"
#include <stdexcept>

/** @details Garante na construção o invariante precoBase > 0 exigido pelo contrato LSP de calcularPreco(). */
Ingresso::Ingresso(double precoBase, Assento* assento)
    : precoBase(precoBase), assento(assento) {
    if (!(precoBase > 0.0)) throw std::invalid_argument("Preco base do ingresso deve ser maior que zero.");
}

void Ingresso::vincularAssento(Assento* assento) {
    this->assento = assento;
}

Assento* Ingresso::getAssento() const {
    return assento;
}

double Ingresso::getPrecoBase() const {
    return precoBase;
}

/** @details Concatena o tipo polimórfico do ingresso ao identificador do assento, caso vinculado. */
std::string Ingresso::getDescricao() const {
    std::string desc = "Ingresso " + getTipo();
    if (assento != nullptr) {
        desc += " (Assento " + assento->getIdentificador() + ")";
    }
    return desc;
}

IngressoInteira::IngressoInteira(double precoBase, Assento* assento)
    : Ingresso(precoBase, assento) {}

double IngressoInteira::calcularPreco() const {
    return getPrecoBase();
}

std::string IngressoInteira::getTipo() const {
    return "Inteira";
}

IngressoMeia::IngressoMeia(double precoBase, Assento* assento)
    : Ingresso(precoBase, assento) {}

double IngressoMeia::calcularPreco() const {
    return getPrecoBase() * 0.5;
}

std::string IngressoMeia::getTipo() const {
    return "Meia-Entrada";
}