#include "Filme.h"

Filme::Filme() : titulo(""), duracao(0), classificacao(0), genero("") {}

Filme::Filme(const std::string& titulo, int duracao, int classificacao, const std::string& genero)
    : titulo(titulo), duracao(duracao), classificacao(classificacao), genero(genero) {}

std::string Filme::getTitulo() const {
    return titulo;
}

int Filme::getDuracao() const {
    return duracao;
}

int Filme::getClassificacao() const {
    return classificacao;
}

std::string Filme::getGenero() const {
    return genero;
}
