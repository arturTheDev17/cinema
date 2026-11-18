/**
 * @file Filme.h
 * @brief Declaração da classe Filme e do operador de inserção em stream associado.
 */

#ifndef FILME_H
#define FILME_H

#include <iosfwd>
#include <string>

/**
 * @class Filme
 * @brief Representa um filme em cartaz com identificador único e invariantes validados.
 * @note O ID automático é gerado por contador estático (instanciar apenas na thread da GUI).
 */
class Filme {
private:
    int id;               ///< Identificador numérico único do filme.
    static int proximoId; ///< Contador estático para geração automática de IDs (uso exclusivo na thread da GUI).
    std::string titulo;   ///< Título do filme (invariante: não vazio).
    int duracao;          ///< Duração em minutos (invariante: > 0).
    int classificacao;    ///< Classificação indicativa em anos (invariante: >= 0).
    std::string genero;   ///< Gênero cinematográfico.

public:
    /**
     * @brief Constrói um filme atribuindo automaticamente o próximo ID disponível.
     * @param[in] titulo Título do filme (não vazio).
     * @param[in] duracao Duração em minutos (> 0).
     * @param[in] classificacao Idade mínima recomendada (>= 0).
     * @param[in] genero Gênero do filme.
     * @throws std::invalid_argument Se titulo for vazio, duracao <= 0 ou classificacao < 0.
     */
    Filme(const std::string& titulo, int duracao, int classificacao, const std::string& genero);

    /**
     * @brief Constrói um filme com ID explícito (sobrecarga usada ao carregar dados persistidos).
     * @param[in] idEspecifico Identificador fixo do filme.
     * @param[in] titulo Título do filme (não vazio).
     * @param[in] duracao Duração em minutos (> 0).
     * @param[in] classificacao Idade mínima recomendada (>= 0).
     * @param[in] genero Gênero do filme.
     * @throws std::invalid_argument Se titulo for vazio, duracao <= 0 ou classificacao < 0.
     * @note Atualiza proximoId caso idEspecifico >= proximoId.
     */
    Filme(int idEspecifico, const std::string& titulo, int duracao, int classificacao, const std::string& genero);

    /** @brief Obtém o ID do filme. @return Inteiro identificador. */
    int getId() const;

    /** @brief Obtém o título do filme. @return String contendo o título. */
    std::string getTitulo() const;

    /** @brief Obtém a duração do filme. @return Duração em minutos. */
    int getDuracao() const;

    /** @brief Obtém a classificação indicativa. @return Idade mínima em anos. */
    int getClassificacao() const;

    /** @brief Obtém o gênero do filme. @return String com o gênero. */
    std::string getGenero() const;

    /**
     * @brief Altera o título do filme.
     * @param[in] novoTitulo Novo título (não pode ser vazio).
     * @throws std::invalid_argument Se novoTitulo for vazio.
     */
    void setTitulo(const std::string& novoTitulo);

    /**
     * @brief Altera a duração do filme.
     * @param[in] novaDuracao Nova duração em minutos (deve ser > 0).
     * @throws std::invalid_argument Se novaDuracao <= 0.
     */
    void setDuracao(int novaDuracao);

    /**
     * @brief Altera a classificação indicativa.
     * @param[in] novaClassificacao Nova idade mínima (deve ser >= 0).
     * @throws std::invalid_argument Se novaClassificacao < 0.
     */
    void setClassificacao(int novaClassificacao);

    /**
     * @brief Altera o gênero do filme.
     * @param[in] novoGenero Novo texto de gênero.
     */
    void setGenero(const std::string& novoGenero);
};

/**
 * @brief Sobrecarga do operador de inserção em stream para exibição resumida do filme.
 * @param[in,out] os Stream de saída.
 * @param[in] filme Objeto Filme a ser formatado.
 * @return Referência para a stream de saída.
 */
std::ostream& operator<<(std::ostream& os, const Filme& filme);

#endif