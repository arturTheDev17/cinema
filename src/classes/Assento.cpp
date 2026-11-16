#include "Assento.h"

Assento::Assento(int fileira, int coluna)
    : fileira(fileira), coluna(coluna), status(SituacaoAssento::LIVRE) {}

int Assento::getFileira() const {
    return fileira;
}

int Assento::getColuna() const {
    return coluna;
}

std::string Assento::getIdentificador() const {
    char letraFileira = static_cast<char>('A' + fileira - 1);
    return std::string(1, letraFileira) + std::to_string(coluna);
}

SituacaoAssento Assento::getStatus() const {
    return status;
}

bool Assento::reservar() {
    if (status != SituacaoAssento::LIVRE) {
        return false;
    }
    status = SituacaoAssento::RESERVADO;
    return true;
}

bool Assento::confirmarReserva() {
    if (status != SituacaoAssento::RESERVADO) {
        return false;
    }
    status = SituacaoAssento::OCUPADO;
    return true;
}

void Assento::liberar() {
    if (status == SituacaoAssento::RESERVADO) {
        status = SituacaoAssento::LIVRE;
    }
}
