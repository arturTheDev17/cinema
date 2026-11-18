/**
 * @file Sessao.h
 * @brief Declaração das enumerações TipoAudio e TecnologiaExibicao e da classe Sessao.
 */

#ifndef SESSAO_H
#define SESSAO_H

#include "Filme.h"
#include "Sala.h"
#include <iosfwd>
#include <string>

/**
 * @enum TipoAudio
 * @brief Modalidades de áudio suportadas em uma sessão.
 */
enum class TipoAudio {
    DUBLADO,   ///< Áudio em português.
    LEGENDADO  ///< Áudio original com legendas.
};

/**
 * @enum TecnologiaExibicao
 * @brief Formatos visuais de projeção disponíveis.
 */
enum class TecnologiaExibicao {
    DOIS_D, ///< Projeção convencional 2D.
    TRES_D  ///< Projeção estereoscópica 3D.
};

/**
 * @class Sessao
 * @brief Representa a exibição de um Filme em uma Sala e horário específicos, com controle de conflitos.
 * @note Guarda cópias de Filme e Sala para manter mapa de lotação independente por sessão.
 */
class Sessao {
private:
    int id;                        ///< Identificador único da sessão.
    Filme filme;                   ///< Cópia do filme exibido nesta sessão.
    Sala sala;                     ///< Cópia da sala com a ocupação de assentos própria desta sessão.
    std::string horario;           ///< Horário de início no formato "HH:MM".
    TipoAudio audio;               ///< Modalidade de áudio da exibição.
    TecnologiaExibicao tecnologia; ///< Tecnologia de projeção (2D ou 3D).
    double precoBase;              ///< Valor base do ingresso em reais (invariante: > 0).

public:
    /**
     * @brief Constrói uma sessão validando formato do horário e preço positivo.
     * @param[in] id Identificador da sessão.
     * @param[in] filme Filme exibido.
     * @param[in] sala Sala modelo copiada para esta sessão.
     * @param[in] horario Horário de início no formato "HH:MM" (00:00 a 23:59).
     * @param[in] audio Modalidade de áudio.
     * @param[in] tecnologia Tecnologia visual.
     * @param[in] precoBase Preço base do ingresso (> 0).
     * @throws std::invalid_argument Se horario não seguir "HH:MM" válido ou se precoBase <= 0.
     */
    Sessao(int id, const Filme& filme, const Sala& sala, const std::string& horario,
           TipoAudio audio, TecnologiaExibicao tecnologia, double precoBase);

    /** @brief Obtém o ID da sessão. @return Inteiro identificador. */
    int getId() const;

    /** @brief Obtém o filme exibido. @return Referência constante para Filme. */
    const Filme& getFilme() const;

    /** @brief Obtém referência mutável para a sala da sessão. @return Referência para Sala. */
    Sala& getSala();

    /** @brief Obtém referência somente leitura para a sala da sessão. @return Referência constante para Sala. */
    const Sala& getSala() const;

    /** @brief Obtém o horário de início. @return String no formato "HH:MM". */
    std::string getHorario() const;

    /** @brief Obtém a modalidade de áudio. @return Valor de TipoAudio. */
    TipoAudio getAudio() const;

    /** @brief Obtém a tecnologia de exibição. @return Valor de TecnologiaExibicao. */
    TecnologiaExibicao getTecnologia() const;

    /** @brief Obtém o preço base do ingresso. @return Valor em reais (> 0). */
    double getPrecoBase() const;

    /**
     * @brief Monta descrição formatada com horário, áudio, tecnologia, número e tipo da sala.
     * @return String resumida da sessão.
     */
    std::string getDescricaoCompleta() const;

    /**
     * @brief Atualiza o horário de início da sessão.
     * @param[in] novoHorario Novo horário no formato "HH:MM".
     * @throws std::invalid_argument Se novoHorario for inválido.
     */
    void setHorario(const std::string& novoHorario);

    /**
     * @brief Substitui a sala da sessão caso nenhum assento esteja reservado ou ocupado.
     * @param[in] novaSala Nova sala modelo.
     * @throws std::logic_error Se temAssentosNaoLivres() for true (evita descartar ingressos já vendidos/em compra).
     */
    void setSala(const Sala& novaSala);

    /** @brief Atualiza a modalidade de áudio da sessão. @param[in] novoAudio Novo TipoAudio. */
    void setAudio(TipoAudio novoAudio);

    /** @brief Atualiza a tecnologia de projeção da sessão. @param[in] novaTecnologia Nova TecnologiaExibicao. */
    void setTecnologia(TecnologiaExibicao novaTecnologia);

    /**
     * @brief Atualiza o preço base do ingresso da sessão.
     * @param[in] novoPreco Novo valor em reais (deve ser > 0).
     * @throws std::invalid_argument Se novoPreco <= 0.
     */
    void setPrecoBase(double novoPreco);

    /**
     * @brief Sincroniza a cópia interna de Filme após edição no catálogo.
     * @param[in] filmeAtualizado Objeto Filme atualizado.
     */
    void atualizarFilme(const Filme& filmeAtualizado);

    /** @brief Converte o horário de início para minutos decorridos desde 00:00. @return Minutos de início. */
    int getInicioMinutos() const;

    /** @brief Calcula o instante de término da sessão (início + duração do filme) em minutos desde 00:00. @return Minutos de término. */
    int getFimMinutos() const;

    /**
     * @brief Verifica se esta sessão se sobrepõe no tempo a outra sessão na mesma sala.
     * @param[in] outra Outra Sessao a verificar.
     * @return true se estiverem na mesma sala e as janelas [início, fim) se sobrepuserem.
     */
    bool conflitaCom(const Sessao& outra) const;

    /**
     * @brief Verifica se há ao menos um assento RESERVADO ou OCUPADO na sala desta sessão.
     * @return true se existem ingressos vendidos ou em processo de compra; false se todos estão livres.
     */
    bool temAssentosNaoLivres() const;

    /**
     * @brief Compara sessões cronologicamente pelo horário ("HH:MM").
     * @param[in] outra Sessão comparada.
     * @return true se o horário desta sessão for anterior ao da outra.
     */
    bool operator<(const Sessao& outra) const;
};

/**
 * @brief Sobrecarga do operador de inserção em stream para exibir resumo da sessão.
 * @param[in,out] os Stream de saída.
 * @param[in] sessao Objeto Sessao a ser formatado.
 * @return Referência para a stream de saída.
 */
std::ostream& operator<<(std::ostream& os, const Sessao& sessao);

#endif