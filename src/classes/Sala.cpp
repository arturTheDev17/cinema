/**
 * @file Sala.cpp
 * @brief Implementação dos métodos de construção, consulta e confirmação da classe Sala.
 */

#include "Sala.h"
#include <stdexcept>

/** @details Valida dimensões dentro de [1..kMaxFileiras] e preenche a matriz de assentos base 1. */
Sala::Sala(int numero, int quantidadeFileiras, int quantidadeColunas, const std::string& tipoSala) {
    if (quantidadeFileiras <= 0 || quantidadeColunas <= 0) {
        throw std::invalid_argument("Dimensoes invalidas da sala.");
    }
    if (quantidadeFileiras > kMaxFileiras) {
        throw std::invalid_argument("Numero maximo de fileiras excedido (A-Z).");
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

/** @details Converte coordenadas base 1 para índices base 0 após validar limites da grade. */
Assento& Sala::getAssento(int fileira, int coluna) {
    if (fileira < 1 || fileira > quantidadeFileiras ||
        coluna < 1 || coluna > quantidadeColunas) {
        throw std::out_of_range("Assento inexistente.");
    }
    return assentos[fileira - 1][coluna - 1];
}

/** @details Sobrecarga const que valida limites da grade e retorna referência somente leitura. */
const Assento& Sala::getAssento(int fileira, int coluna) const {
    if (fileira < 1 || fileira > quantidadeFileiras ||
        coluna < 1 || coluna > quantidadeColunas) {
        throw std::out_of_range("Assento inexistente.");
    }
    return assentos[static_cast<size_t>(fileira - 1)][static_cast<size_t>(coluna - 1)];
}

/** @details Percorre toda a matriz somando os assentos com status LIVRE. */
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

/** @details Efetiva a ocupação definitiva de todos os assentos atualmente marcados como RESERVADO. */
void Sala::confirmarAssentosReservados() {
    for (auto& linha : assentos) {
        for (auto& assento : linha) {
            if (assento.getStatus() == SituacaoAssento::RESERVADO) {
                assento.confirmarReserva();
            }
        }
    }
}