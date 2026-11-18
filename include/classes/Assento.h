/**
 * @file Assento.h
 * @brief Declaração da enumeração SituacaoAssento e da classe Assento.
 */

#ifndef ASSENTO_H
#define ASSENTO_H

#include <string>

/**
 * @enum SituacaoAssento
 * @brief Representa os possíveis estados do ciclo de vida de um assento na sala.
 */
enum class SituacaoAssento {
    LIVRE,      ///< Assento disponível para escolha.
    RESERVADO,  ///< Assento temporariamente selecionado em uma compra em andamento.
    OCUPADO     ///< Assento com compra confirmada e paga definitivamente.
};

/**
 * @class Assento
 * @brief Modela um assento de uma sala de exibição e controla suas transições de estado.
 * @note Ciclo de vida: LIVRE -> RESERVADO (escolhido) -> OCUPADO (compra paga).
 */
class Assento {
private:
    int fileira;            ///< Número da fileira (base 1, mapeado de 'A' até 'Z').
    int coluna;             ///< Número da poltrona na fileira (base 1).
    SituacaoAssento status; ///< Estado atual de disponibilidade do assento.

public:
    /**
     * @brief Constrói um assento inicialmente no estado LIVRE.
     * @param[in] fileira Índice da fileira (iniciando em 1).
     * @param[in] coluna Índice da coluna (iniciando em 1).
     */
    Assento(int fileira, int coluna);

    /** @brief Obtém o número da fileira do assento. @return Inteiro correspondente à fileira. */
    int getFileira() const;

    /** @brief Obtém o número da coluna do assento. @return Inteiro correspondente à coluna. */
    int getColuna() const;

    /**
     * @brief Gera o identificador legível do assento no formato Letra+Número (ex.: "A1", "C4").
     * @return String com o identificador do assento.
     */
    std::string getIdentificador() const;

    /** @brief Consulta o estado atual do assento. @return Valor da enumeração SituacaoAssento. */
    SituacaoAssento getStatus() const;

    /**
     * @brief Reserva temporariamente o assento caso esteja LIVRE.
     * @return true se a transição de LIVRE para RESERVADO ocorreu; false caso contrário.
     */
    bool reservar();

    /**
     * @brief Efetiva a ocupação definitiva de um assento que estava RESERVADO.
     * @return true se a transição de RESERVADO para OCUPADO ocorreu; false caso contrário.
     */
    bool confirmarReserva();

    /**
     * @brief Devolve o assento ao estado LIVRE caso esteja atualmente RESERVADO.
     * @note Assentos no estado OCUPADO não são alterados por este método.
     */
    void liberar();
};

#endif