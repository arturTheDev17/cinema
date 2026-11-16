#ifndef ASSENTO_H
#define ASSENTO_H

#include <string>

enum class SituacaoAssento {
    LIVRE,
    RESERVADO,
    OCUPADO
};

class Assento {
private:
    int fileira;
    int coluna;
    SituacaoAssento status;

public:
    Assento(int fileira, int coluna);

    int getFileira() const;
    int getColuna() const;
    std::string getIdentificador() const;
    SituacaoAssento getStatus() const;

    bool reservar();
    bool confirmarReserva();
    void liberar();
};

#endif
