#ifndef INGRESSO_H
#define INGRESSO_H

#include "Assento.h"
#include <string>

class Ingresso {
protected:
    double precoBase;
    Assento* assento;

public:
    Ingresso(double precoBase, Assento* assento = nullptr);
    virtual ~Ingresso() = default;

    void vincularAssento(Assento* assento);
    Assento* getAssento() const;
    double getPrecoBase() const;

    virtual double calcularPreco() const = 0;
    virtual std::string getTipo() const = 0;
    virtual std::string getDescricao() const;
};

class IngressoInteira : public Ingresso {
public:
    IngressoInteira(double precoBase, Assento* assento = nullptr);
    double calcularPreco() const override;
    std::string getTipo() const override;
};

class IngressoMeia : public Ingresso {
public:
    IngressoMeia(double precoBase, Assento* assento = nullptr);
    double calcularPreco() const override;
    std::string getTipo() const override;
};

#endif
