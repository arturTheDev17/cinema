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

IngressoVIP::IngressoVIP(double precoBase, double adicionalVIP, Assento* assento)
    : Ingresso(precoBase, assento), adicionalVIP(adicionalVIP) {}

double IngressoVIP::calcularPreco() const {
    return precoBase + adicionalVIP;
}

std::string IngressoVIP::getTipo() const {
    return "VIP (Inteira)";
}

IngressoMeiaVIP::IngressoMeiaVIP(double precoBase, double adicionalVIP, Assento* assento)
    : Ingresso(precoBase, assento), adicionalVIP(adicionalVIP) {}

double IngressoMeiaVIP::calcularPreco() const {
    return (precoBase * 0.5) + adicionalVIP; // Meia do valor base + taxa VIP
}

std::string IngressoMeiaVIP::getTipo() const {
    return "VIP (Meia-Entrada)";
}