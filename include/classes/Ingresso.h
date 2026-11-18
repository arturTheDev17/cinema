/**
 * @file Ingresso.h
 * @brief Declaração da classe abstrata Ingresso e das especializações IngressoInteira e IngressoMeia.
 */

#ifndef INGRESSO_H
#define INGRESSO_H

#include "Assento.h"
#include <string>

/**
 * @class Ingresso
 * @brief Base abstrata dos ingressos com polimorfismo dinâmico e contrato LSP explícito.
 * @note Guarda um ponteiro não-proprietário para Assento. Construtor e atribuição de cópia
 *       são protegidos para evitar fatiamento (object slicing) via Ingresso&.
 */
class Ingresso {
private:
    double precoBase; ///< Preço base da sessão (invariante: > 0).
    Assento* assento; ///< Ponteiro não-proprietário para o assento vinculado (pode ser nullptr).

protected:
    /** @brief Construtor de cópia protegido para evitar fatiamento polimórfico. */
    Ingresso(const Ingresso&) = default;

    /** @brief Operador de atribuição por cópia protegido para evitar fatiamento polimórfico. */
    Ingresso& operator=(const Ingresso&) = default;

public:
    /**
     * @brief Constrói um ingresso validando que o preço base é estritamente positivo.
     * @param[in] precoBase Preço base da sessão em reais (deve ser > 0).
     * @param[in] assento Ponteiro opcional para o assento já selecionado.
     * @throws std::invalid_argument Se precoBase <= 0.
     */
    Ingresso(double precoBase, Assento* assento = nullptr);

    /** @brief Destrutor virtual padrão para destruição polimórfica segura via ponteiro base. */
    virtual ~Ingresso() = default;

    /**
     * @brief Vincula ou altera o assento associado a este ingresso.
     * @param[in] assento Ponteiro para Assento (ou nullptr para desvincular).
     */
    void vincularAssento(Assento* assento);

    /** @brief Obtém o assento vinculado. @return Ponteiro para Assento ou nullptr. */
    Assento* getAssento() const;

    /** @brief Obtém o preço base da sessão atribuído ao ingresso. @return Valor > 0 em reais. */
    double getPrecoBase() const;

    /**
     * @brief Calcula o preço final do ingresso conforme a modalidade da subclasse.
     * @return Preço final do bilhete em reais.
     * @pre precoBase > 0 (garantido pelo construtor da classe base).
     * @post 0 < resultado <= getPrecoBase(); método const (não altera o estado do ingresso).
     * @note Contrato LSP: subclasses não podem exigir pré-condições extras (como assento já vinculado)
     *       nem violar a faixa de retorno, permitindo ao Carrinho tratar qualquer Ingresso igualmente.
     */
    virtual double calcularPreco() const = 0;

    /**
     * @brief Retorna o nome da modalidade do ingresso.
     * @return String identificando o tipo ("Inteira" ou "Meia-Entrada").
     */
    virtual std::string getTipo() const = 0;

    /**
     * @brief Monta a descrição textual completa do ingresso, incluindo o assento caso vinculado.
     * @return String descritiva formatada para o resumo do pedido.
     */
    virtual std::string getDescricao() const;
};

/**
 * @class IngressoInteira
 * @brief Especialização de Ingresso que cobra 100% do preço base.
 */
class IngressoInteira : public Ingresso {
public:
    /** @brief Constrói um ingresso inteiro. @param[in] precoBase Preço base (> 0). @param[in] assento Assento opcional. */
    IngressoInteira(double precoBase, Assento* assento = nullptr);

    /** @brief Retorna 100% de getPrecoBase(). @return Preço final em reais. */
    double calcularPreco() const override;

    /** @brief Retorna "Inteira". @return Tipo do ingresso. */
    std::string getTipo() const override;
};

/**
 * @class IngressoMeia
 * @brief Especialização de Ingresso que cobra 50% do preço base.
 */
class IngressoMeia : public Ingresso {
public:
    /** @brief Constrói um ingresso meia-entrada. @param[in] precoBase Preço base (> 0). @param[in] assento Assento opcional. */
    IngressoMeia(double precoBase, Assento* assento = nullptr);

    /** @brief Retorna 50% de getPrecoBase(). @return Preço final em reais. */
    double calcularPreco() const override;

    /** @brief Retorna "Meia-Entrada". @return Tipo do ingresso. */
    std::string getTipo() const override;
};

#endif