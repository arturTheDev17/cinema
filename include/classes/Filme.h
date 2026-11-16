#ifndef FILME_H
#define FILME_H

#include <string>

class Filme {
private:
    int id;
    static int proximoId; // contador estatico (so use na thread da GUI)
    std::string titulo;
    int duracao;
    int classificacao;
    std::string genero;

public:
    Filme();
    Filme(const std::string& titulo, int duracao, int classificacao, const std::string& genero);

    int getId() const;
    std::string getTitulo() const;
    int getDuracao() const;
    int getClassificacao() const;
    std::string getGenero() const;
};

#endif
