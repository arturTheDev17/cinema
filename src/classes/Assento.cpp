/**
 * @file Assento.cpp
 * @brief Implementação dos métodos e transições de estado da classe Assento.
 */

#include "Assento.h"

/** @details Inicializa o assento na posição informada com status LIVRE. */
Assento::Assento(int fileira, int coluna)
    : fileira(fileira), coluna(coluna), status(SituacaoAssento::LIVRE) {}

int Assento::getFileira() const {
    return fileira;
}

int Assento::getColuna() const {
    return coluna;
}

/** @details Converte o índice base-1 da fileira para letra maiúscula ('A'..) concatenada à coluna. */
std::string Assento::getIdentificador() const {
    char letraFileira = static_cast<char>('A' + fileira - 1);
    return std::string(1, letraFileira) + std::to_string(coluna);
}

SituacaoAssento Assento::getStatus() const {
    return status;
}

/** @details Só transita para RESERVADO se o status atual for LIVRE. */
bool Assento::reservar() {
    if (status != SituacaoAssento::LIVRE) {
        return false;
    }
    status = SituacaoAssento::RESERVADO;
    return true;
}

/** @details Só transita para OCUPADO se o status atual for RESERVADO. */
bool Assento::confirmarReserva() {
    if (status != SituacaoAssento::RESERVADO) {
        return false;
    }
    status = SituacaoAssento::OCUPADO;
    return true;
}

/** @details Restaura para LIVRE apenas assentos no estado RESERVADO (mantém OCUPADO intacto). */
void Assento::liberar() {
    if (status == SituacaoAssento::RESERVADO) {
        status = SituacaoAssento::LIVRE;
    }
}