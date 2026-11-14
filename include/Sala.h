#ifndef SALA_H
#define SALA_H

#include "Assento.h"
#include <vector>
#include <string>

class Sala {
private:
    int numero;
    std::string tipoSala;
    int quantidadeFileiras;
    int quantidadeColunas;
    std::vector<std::vector<Assento>> assentos;

public:
    Sala(int numero, int quantidadeFileiras, int quantidadeColunas, const std::string& tipoSala = "Padrao");

    int getNumero() const;
    std::string getTipoSala() const;
    int getQuantidadeFileiras() const;
    int getQuantidadeColunas() const;
    int getCapacidade() const;

    Assento& getAssento(int fileira, int coluna);
    int contarAssentosLivres() const;
    void confirmarAssentosReservados();
};

#endif
