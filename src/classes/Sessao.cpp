/**
 * @file Sessao.cpp
 * @brief Implementação da classe Sessao, validações de horário/conflito e operador de stream.
 */

#include "Sessao.h"
#include <cctype>
#include <cstdio>
#include <ostream>
#include <stdexcept>

namespace {
/**
 * @brief Valida se a string segue o formato estrito "HH:MM" entre 00:00 e 23:59.
 * @param[in] h String de horário a verificar.
 * @return true se o formato e os valores numéricos forem válidos; false caso contrário.
 */
bool horarioValido(const std::string& h) {
    if (h.size() != 5 || h[2] != ':') return false;
    for (size_t i : {0u, 1u, 3u, 4u}) {
        if (!std::isdigit(static_cast<unsigned char>(h[i]))) return false;
    }
    return std::stoi(h.substr(0, 2)) <= 23 && std::stoi(h.substr(3, 2)) <= 59;
}
}

/** @details Valida formato de horário e preço positivo na inicialização da sessão. */
Sessao::Sessao(int id, const Filme& filme, const Sala& sala, const std::string& horario,
               TipoAudio audio, TecnologiaExibicao tecnologia, double precoBase)
    : id(id), filme(filme), sala(sala), horario(horario),
      audio(audio), tecnologia(tecnologia), precoBase(precoBase) {
    if (!horarioValido(horario)) throw std::invalid_argument("Horario invalido (use HH:MM).");
    if (!(precoBase > 0.0)) throw std::invalid_argument("Preco deve ser maior que zero.");
}

int Sessao::getId() const {
    return id;
}

const Filme& Sessao::getFilme() const {
    return filme;
}

Sala& Sessao::getSala() {
    return sala;
}

const Sala& Sessao::getSala() const {
    return sala;
}

std::string Sessao::getHorario() const {
    return horario;
}

TipoAudio Sessao::getAudio() const {
    return audio;
}

TecnologiaExibicao Sessao::getTecnologia() const {
    return tecnologia;
}

double Sessao::getPrecoBase() const {
    return precoBase;
}

/** @details Concatena horário, áudio, projeção (2D/3D) e identificação da sala. */
std::string Sessao::getDescricaoCompleta() const {
    std::string strAudio = (audio == TipoAudio::DUBLADO) ? "Dublado" : "Legendado";
    std::string strTec = (tecnologia == TecnologiaExibicao::TRES_D) ? "3D" : "2D";
    return horario + " | " + strAudio + " | " + strTec +
           " | Sala " + std::to_string(sala.getNumero()) + " (" + sala.getTipoSala() + ")";
}

namespace {
/**
 * @brief Converte uma string "HH:MM" para total de minutos desde 00:00.
 * @param[in] hhmm Horário formatado.
 * @return Minutos decorridos desde meia-noite (0 em caso de falha de leitura).
 */
int minutosDe(const std::string& hhmm) {
    int h = 0, m = 0;
    if (std::sscanf(hhmm.c_str(), "%d:%d", &h, &m) != 2) return 0;
    return h * 60 + m;
}
}

void Sessao::setHorario(const std::string& novoHorario) {
    if (!horarioValido(novoHorario)) throw std::invalid_argument("Horario invalido (use HH:MM).");
    horario = novoHorario;
}

/** @details Impede a troca de sala caso já existam assentos reservados ou ocupados para preservar a integridade dos ingressos. */
void Sessao::setSala(const Sala& novaSala) {
    // Trocar a sala descartaria o mapa de assentos (e os ingressos já vendidos).
    if (temAssentosNaoLivres()) throw std::logic_error("Sessao com assentos reservados/ocupados nao pode trocar de sala.");
    sala = novaSala;
}
void Sessao::setAudio(TipoAudio novoAudio) { audio = novoAudio; }
void Sessao::setTecnologia(TecnologiaExibicao novaTecnologia) { tecnologia = novaTecnologia; }
void Sessao::atualizarFilme(const Filme& filmeAtualizado) { filme = filmeAtualizado; }

void Sessao::setPrecoBase(double novoPreco) {
    if (novoPreco <= 0.0) throw std::invalid_argument("Preco deve ser maior que zero.");
    precoBase = novoPreco;
}

int Sessao::getInicioMinutos() const { return minutosDe(horario); }
int Sessao::getFimMinutos() const { return getInicioMinutos() + filme.getDuracao(); }

/** @details Verifica intersecção dos intervalos semiabertos [início, fim) quando ambas pertencem à mesma sala. */
bool Sessao::conflitaCom(const Sessao& outra) const {
    return sala.getNumero() == outra.sala.getNumero()
        && getInicioMinutos() < outra.getFimMinutos()
        && outra.getInicioMinutos() < getFimMinutos();
}

/** @details Compara a contagem atual de assentos livres com a capacidade total da sala. */
bool Sessao::temAssentosNaoLivres() const {
    return sala.contarAssentosLivres() != sala.getCapacidade();
}

bool Sessao::operator<(const Sessao& outra) const {
    return horario < outra.horario;
}

std::ostream& operator<<(std::ostream& os, const Sessao& s) {
    return os << "[Sessao ID " << s.getId() << "] Filme: " << s.getFilme().getTitulo()
              << " | Horario: " << s.getHorario() << " | Preco Base: R$ " << s.getPrecoBase();
}