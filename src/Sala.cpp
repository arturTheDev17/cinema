#include "Sala.h"
#include <stdexcept>

Sala::Sala(int numero, int quantidadeFileiras, int quantidadeColunas, const std::string& tipoSala) {
    if (quantidadeFileiras <= 0 || quantidadeColunas <= 0) {
        throw std::invalid_argument("Dimensoes invalidas da sala.");
    }
    this->numero = numero;
    this->tipoSala = tipoSala;
    this->quantidadeFileiras = quantidadeFileiras;
    this->quantidadeColunas = quantidadeColunas;

    for (int i = 0; i < quantidadeFileiras; i++) {
        std::vector<Assento> linha;
        for (int j = 0; j < quantidadeColunas; j++) {
            linha.emplace_back(i + 1, j + 1);
        }
        assentos.push_back(linha);
    }
}

int Sala::getNumero() const {
    return numero;
}

std::string Sala::getTipoSala() const {
    return tipoSala;
}

int Sala::getQuantidadeFileiras() const {
    return quantidadeFileiras;
}

int Sala::getQuantidadeColunas() const {
    return quantidadeColunas;
}

int Sala::getCapacidade() const {
    return quantidadeFileiras * quantidadeColunas;
}

Assento& Sala::getAssento(int fileira, int coluna) {
    if (fileira < 1 || fileira > quantidadeFileiras ||
        coluna < 1 || coluna > quantidadeColunas) {
        throw std::out_of_range("Assento inexistente.");
    }
    return assentos[fileira - 1][coluna - 1];
}

int Sala::contarAssentosLivres() const {
    int livres = 0;
    for (const auto& linha : assentos) {
        for (const auto& assento : linha) {
            if (assento.getStatus() == SituacaoAssento::LIVRE) {
                livres++;
            }
        }
    }
    return livres;
}

void Sala::confirmarAssentosReservados() {
    for (auto& linha : assentos) {
        for (auto& assento : linha) {
            if (assento.getStatus() == SituacaoAssento::RESERVADO) {
                assento.confirmarReserva();
            }
        }
    }
}
