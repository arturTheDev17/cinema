#include "Ingresso.h"

Ingresso::Ingresso(double precoBase, Assento* assento)
    : precoBase(precoBase), assento(assento) {}

void Ingresso::vincularAssento(Assento* assento) {
    this->assento = assento;
}

Assento* Ingresso::getAssento() const {
    return assento;
}

double Ingresso::getPrecoBase() const {
    return precoBase;
}

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
    return precoBase;
}

std::string IngressoInteira::getTipo() const {
    return "Inteira";
}

IngressoMeia::IngressoMeia(double precoBase, Assento* assento)
    : Ingresso(precoBase, assento) {}

double IngressoMeia::calcularPreco() const {
    return precoBase * 0.5;
}

std::string IngressoMeia::getTipo() const {
    return "Meia-Entrada";
}
