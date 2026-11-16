#include "Filme.h"

int Filme::proximoId = 1;

// id 0 = filme "vazio"/invalido (construtor padrao).
Filme::Filme() : id(0), titulo(""), duracao(0), classificacao(0), genero("") {}

Filme::Filme(const std::string& titulo, int duracao, int classificacao, const std::string& genero)
    : id(proximoId++), titulo(titulo), duracao(duracao), classificacao(classificacao), genero(genero) {}

int Filme::getId() const {
    return id;
}

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
