#ifndef REPOSITORIO_GENERICO_H
#define REPOSITORIO_GENERICO_H

#include <vector>
#include <stdexcept>
#include <functional>

template <typename T>
class RepositorioGenerico {
private:
    std::vector<T> itens;

public:
    void adicionar(const T& item) {
        itens.push_back(item);
    }

    void removerPorIndice(size_t indice) {
        if (indice >= itens.size()) {
            throw std::out_of_range("Indice fora dos limites.");
        }
        itens.erase(itens.begin() + indice);
    }

    void limpar() {
        itens.clear();
    }

    size_t tamanho() const {
        return itens.size();
    }

    bool vazio() const {
        return itens.empty();
    }

    T& obter(size_t indice) {
        if (indice >= itens.size()) {
            throw std::out_of_range("Indice fora dos limites.");
        }
        return itens[indice];
    }

    const T& obter(size_t indice) const {
        if (indice >= itens.size()) {
            throw std::out_of_range("Indice fora dos limites.");
        }
        return itens[indice];
    }

    const std::vector<T>& listarTodos() const {
        return itens;
    }

    std::vector<T> filtrar(std::function<bool(const T&)> predicado) const {
        std::vector<T> resultado;
        for (const auto& item : itens) {
            if (predicado(item)) {
                resultado.push_back(item);
            }
        }
        return resultado;
    }
};

#endif
