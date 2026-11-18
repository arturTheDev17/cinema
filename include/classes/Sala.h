/**
 * @file Sala.h
 * @brief Declaração da classe Sala, responsável pela matriz de assentos de uma sessão.
 */

#ifndef SALA_H
#define SALA_H

#include "Assento.h"
#include <vector>
#include <string>

/**
 * @class Sala
 * @brief Sala de exibição com grade bidimensional de objetos Assento.
 * @note Segue a Regra do Zero: como encapsula std::vector sem ponteiros próprios, a cópia padrão
 *       é correta e garante que cada Sessao possua lotação independente.
 */
class Sala {
public:
    static constexpr int kMaxFileiras = 26; ///< Limite máximo de fileiras suportado (identificadores de 'A' até 'Z').

private:
    int numero;                                 ///< Número identificador da sala.
    std::string tipoSala;                       ///< Categoria da sala (ex.: "Padrão", "3D", "VIP").
    int quantidadeFileiras;                     ///< Total de fileiras da sala.
    int quantidadeColunas;                      ///< Total de assentos por fileira.
    std::vector<std::vector<Assento>> assentos; ///< Matriz bidimensional de assentos (composição).

public:
    /**
     * @brief Constrói uma sala e inicializa todos os assentos como LIVRE.
     * @param[in] numero Número identificador da sala.
     * @param[in] quantidadeFileiras Quantidade de fileiras (1 até kMaxFileiras).
     * @param[in] quantidadeColunas Quantidade de colunas (> 0).
     * @param[in] tipoSala Categoria da sala (padrão: "Padrao").
     * @throws std::invalid_argument Se dimensões <= 0 ou quantidadeFileiras > kMaxFileiras.
     */
    Sala(int numero, int quantidadeFileiras, int quantidadeColunas, const std::string& tipoSala = "Padrao");

    /** @brief Obtém o número da sala. @return Número identificador. */
    int getNumero() const;

    /** @brief Obtém a categoria da sala. @return String com o tipo da sala. */
    std::string getTipoSala() const;

    /** @brief Obtém a quantidade de fileiras. @return Total de fileiras. */
    int getQuantidadeFileiras() const;

    /** @brief Obtém a quantidade de colunas. @return Total de colunas por fileira. */
    int getQuantidadeColunas() const;

    /** @brief Calcula a capacidade máxima de espectadores. @return Produto fileiras * colunas. */
    int getCapacidade() const;

    /**
     * @brief Acesso mutável a um assento específico pela posição.
     * @param[in] fileira Número da fileira (base 1).
     * @param[in] coluna Número da coluna (base 1).
     * @return Referência mutável para Assento.
     * @throws std::out_of_range Caso os índices estejam fora dos limites da sala.
     */
    Assento& getAssento(int fileira, int coluna);

    /**
     * @brief Acesso somente leitura a um assento específico pela posição.
     * @param[in] fileira Número da fileira (base 1).
     * @param[in] coluna Número da coluna (base 1).
     * @return Referência constante para Assento.
     * @throws std::out_of_range Caso os índices estejam fora dos limites da sala.
     */
    const Assento& getAssento(int fileira, int coluna) const;

    /** @brief Conta quantos assentos ainda estão com status LIVRE. @return Total de assentos disponíveis. */
    int contarAssentosLivres() const;

    /** @brief Converte todos os assentos RESERVADO da sala para o estado OCUPADO. */
    void confirmarAssentosReservados();
};

#endif