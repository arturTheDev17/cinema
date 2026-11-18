/**
 * @file RepositorioGenerico.h
 * @brief Declaração e implementação da classe template RepositorioGenerico.
 */

#ifndef REPOSITORIO_GENERICO_H
#define REPOSITORIO_GENERICO_H

#include <cstddef>
#include <deque>
#include <vector>
#include <stdexcept>
#include <functional>

/**
 * @class RepositorioGenerico
 * @tparam T Tipo das entidades gerenciadas pelo repositório (deve ser copiável).
 * @brief Contêiner genérico em memória baseado em std::deque com busca sobrecarregada e filtragem.
 *
 * @note Utiliza std::deque propositalmente para que adicionar() nunca invalide ponteiros ou referências
 *       a elementos já armazenados (essencial porque o Carrinho mantém Sessao* durante cadastros).
 *       Entretanto, removerPorIndice() e removerPorId() invalidam referências ao item removido e seguintes.
 * @note Métodos que utilizam identificador (removerPorId, buscar(int)) exigem que T implemente
 *       `int getId() const` (satisfeito por Filme, Sessao e ItemBomboniere). Caso T não possua getId(),
 *       o restante do repositório continua utilizável pois membros de templates só são instanciados sob demanda.
 */
template <typename T>
class RepositorioGenerico {
private:
    std::deque<T> itens; ///< Armazenamento em deque que preserva estabilidade de ponteiros em inserções no fim.

public:
    /**
     * @brief Guarda uma cópia do item ao final do repositório sem invalidar referências existentes.
     * @param[in] item Objeto do tipo T a ser armazenado.
     */
    void adicionar(const T& item) {
        itens.push_back(item);
    }

    /**
     * @brief Remove o elemento na posição indicada.
     * @param[in] indice Posição baseada em zero.
     * @throws std::out_of_range Se indice >= tamanho().
     * @note Invalida ponteiros/referências para o elemento removido e subsequentes.
     */
    void removerPorIndice(size_t indice) {
        if (indice >= itens.size()) {
            throw std::out_of_range("Indice fora dos limites.");
        }
        itens.erase(itens.begin() + indice);
    }

    /**
     * @brief Remove o primeiro elemento cujo getId() seja igual ao ID informado.
     * @param[in] id Identificador do elemento a remover.
     * @return true se o elemento foi encontrado e removido; false caso contrário.
     * @note Requer que T implemente getId(). Invalida ponteiros para o item removido e seguintes.
     */
    bool removerPorId(int id) {
        for (auto it = itens.begin(); it != itens.end(); ++it) {
            if (it->getId() == id) {
                itens.erase(it);
                return true;
            }
        }
        return false;
    }

    /** @brief Remove todos os elementos armazenados no repositório. */
    void limpar() {
        itens.clear();
    }

    /** @brief Retorna a quantidade de elementos guardados. @return Número de itens. */
    size_t tamanho() const {
        return itens.size();
    }

    /** @brief Verifica se o repositório está vazio. @return true se vazio; false caso contrário. */
    bool vazio() const {
        return itens.empty();
    }

    /**
     * @brief Acessa por referência mutável o elemento na posição informada.
     * @param[in] indice Posição baseada em zero (0 .. tamanho()-1).
     * @return Referência para T.
     * @throws std::out_of_range Se indice >= tamanho().
     */
    T& obter(size_t indice) {
        if (indice >= itens.size()) {
            throw std::out_of_range("Indice fora dos limites.");
        }
        return itens[indice];
    }

    /**
     * @brief Acessa por referência somente leitura o elemento na posição informada.
     * @param[in] indice Posição baseada em zero (0 .. tamanho()-1).
     * @return Referência constante para T.
     * @throws std::out_of_range Se indice >= tamanho().
     */
    const T& obter(size_t indice) const {
        if (indice >= itens.size()) {
            throw std::out_of_range("Indice fora dos limites.");
        }
        return itens[indice];
    }

    /** @brief Retorna todos os elementos na ordem de cadastro. @return Referência constante para o deque interno. */
    const std::deque<T>& listarTodos() const {
        return itens;
    }

    /**
     * @brief Busca mutável por identificador (sobrecarga de polimorfismo estático).
     * @param[in] id Identificador procurado.
     * @return Ponteiro para o elemento encontrado ou nullptr se não existir.
     * @note Requer T::getId().
     */
    T* buscar(int id) {
        for (auto& item : itens) {
            if (item.getId() == id) return &item;
        }
        return nullptr;
    }

    /**
     * @brief Busca somente leitura por identificador (sobrecarga de polimorfismo estático).
     * @param[in] id Identificador procurado.
     * @return Ponteiro constante para o elemento encontrado ou nullptr se não existir.
     * @note Requer T::getId().
     */
    const T* buscar(int id) const {
        for (const auto& item : itens) {
            if (item.getId() == id) return &item;
        }
        return nullptr;
    }

    /**
     * @brief Busca mutável por critério arbitrário (sobrecarga de polimorfismo estático).
     * @param[in] predicado Função que recebe const T& e retorna true para o elemento desejado.
     * @return Ponteiro para o primeiro elemento compatível ou nullptr se nenhum satisfizer.
     * @note Não exige que T implemente getId().
     */
    T* buscar(const std::function<bool(const T&)>& predicado) {
        for (auto& item : itens) {
            if (predicado(item)) return &item;
        }
        return nullptr;
    }

    /**
     * @brief Busca somente leitura por critério arbitrário (sobrecarga de polimorfismo estático).
     * @param[in] predicado Função que recebe const T& e retorna true para o elemento desejado.
     * @return Ponteiro constante para o primeiro elemento compatível ou nullptr se nenhum satisfizer.
     * @note Não exige que T implemente getId().
     */
    const T* buscar(const std::function<bool(const T&)>& predicado) const {
        for (const auto& item : itens) {
            if (predicado(item)) return &item;
        }
        return nullptr;
    }

    /**
     * @brief Verifica se existe ao menos um elemento que satisfaça o predicado informado.
     * @param[in] predicado Critério booleano de avaliação.
     * @return true se existir pelo menos um elemento compatível; false caso contrário.
     */
    bool existe(const std::function<bool(const T&)>& predicado) const {
        return buscar(predicado) != nullptr;
    }

    /**
     * @brief Retorna ponteiros para todos os elementos que satisfazem o critério, sem copiá-los.
     * @param[in] predicado Função de filtragem aplicada a cada elemento.
     * @return Vetor de ponteiros constantes válidos enquanto não houver remoção no repositório.
     */
    std::vector<const T*> filtrar(const std::function<bool(const T&)>& predicado) const {
        std::vector<const T*> resultado;
        for (const auto& item : itens) {
            if (predicado(item)) resultado.push_back(&item);
        }
        return resultado;
    }
};

#endif