/**
 * @file Filme.cpp
 * @brief Implementação da classe Filme, validadores de invariantes e operador de stream.
 */

#include "Filme.h"
#include <ostream>
#include <stdexcept>

int Filme::proximoId = 1;

namespace {
/**
 * @brief Valida os invariantes comuns de construção de um objeto Filme.
 * @param[in] titulo Título do filme (não pode ser vazio).
 * @param[in] duracao Duração em minutos (deve ser > 0).
 * @param[in] classificacao Classificação indicativa em anos (deve ser >= 0).
 * @throws std::invalid_argument Caso qualquer invariante seja violado.
 */
void validarDadosFilme(const std::string& titulo, int duracao, int classificacao) {
    if (titulo.empty()) throw std::invalid_argument("Titulo vazio.");
    if (duracao <= 0) throw std::invalid_argument("Duracao invalida.");
    if (classificacao < 0) throw std::invalid_argument("Classificacao invalida.");
}
}

/** @details Valida os dados antes de incrementar proximoId, garantindo que falhas não desperdicem IDs. */
Filme::Filme(const std::string& titulo, int duracao, int classificacao, const std::string& genero)
    : id(0), titulo(titulo), duracao(duracao), classificacao(classificacao), genero(genero) {
    validarDadosFilme(titulo, duracao, classificacao);
    id = proximoId++; // Só consome um id se os dados forem válidos
}

/** @details Valida os dados e sincroniza proximoId caso o ID carregado seja maior ou igual ao atual. */
Filme::Filme(int idEspecifico, const std::string& titulo, int duracao, int classificacao, const std::string& genero)
    : id(idEspecifico), titulo(titulo), duracao(duracao), classificacao(classificacao), genero(genero) {
    validarDadosFilme(titulo, duracao, classificacao);
    if (idEspecifico >= proximoId) proximoId = idEspecifico + 1;
}

void Filme::setTitulo(const std::string& novoTitulo) {
    if (novoTitulo.empty()) throw std::invalid_argument("Titulo vazio.");
    titulo = novoTitulo;
}

void Filme::setDuracao(int novaDuracao) {
    if (novaDuracao <= 0) throw std::invalid_argument("Duracao invalida.");
    duracao = novaDuracao;
}

void Filme::setClassificacao(int novaClassificacao) {
    if (novaClassificacao < 0) throw std::invalid_argument("Classificacao invalida.");
    classificacao = novaClassificacao;
}

void Filme::setGenero(const std::string& novoGenero) {
    genero = novoGenero;
}

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

/** @details Formata o filme para relatórios de console no padrão "[ID x] Título (Gênero, X min)". */
std::ostream& operator<<(std::ostream& os, const Filme& f) {
    return os << "[ID " << f.getId() << "] " << f.getTitulo() << " (" << f.getGenero()
              << ", " << f.getDuracao() << " min)";
}