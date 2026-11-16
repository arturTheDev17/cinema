#ifndef SESSAO_H
#define SESSAO_H

#include "Filme.h"
#include "Sala.h"
#include <string>

enum class TipoAudio {
    DUBLADO,
    LEGENDADO
};

enum class TecnologiaExibicao {
    _2D,
    _3D
};

class Sessao {
private:
    int id;
    Filme filme;
    Sala sala;
    std::string horario;
    TipoAudio audio;
    TecnologiaExibicao tecnologia;
    double precoBase;

public:
    Sessao(int id, const Filme& filme, const Sala& sala, const std::string& horario,
           TipoAudio audio, TecnologiaExibicao tecnologia, double precoBase);

    int getId() const;
    const Filme& getFilme() const;
    Sala& getSala();
    const Sala& getSala() const;
    std::string getHorario() const;
    TipoAudio getAudio() const;
    TecnologiaExibicao getTecnologia() const;
    double getPrecoBase() const;

    std::string getDescricaoCompleta() const;
};

#endif
