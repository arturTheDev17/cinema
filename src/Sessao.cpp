#include "Sessao.h"

Sessao::Sessao(int id, const Filme& filme, const Sala& sala, const std::string& horario,
               TipoAudio audio, TecnologiaExibicao tecnologia, double precoBase)
    : id(id), filme(filme), sala(sala), horario(horario),
      audio(audio), tecnologia(tecnologia), precoBase(precoBase) {}

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

std::string Sessao::getDescricaoCompleta() const {
    std::string strAudio = (audio == TipoAudio::DUBLADO) ? "Dublado" : "Legendado";
    std::string strTec = (tecnologia == TecnologiaExibicao::_3D) ? "3D" : "2D";
    return horario + " | " + strAudio + " | " + strTec +
           " | Sala " + std::to_string(sala.getNumero()) + " (" + sala.getTipoSala() + ")";
}
