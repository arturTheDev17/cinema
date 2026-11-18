/**
 * @file Algoritmos.h
 * @brief Funções utilitárias genéricas (templates) reutilizáveis no domínio.
 */

#ifndef ALGORITMOS_H
#define ALGORITMOS_H

/**
 * @brief Acumula a soma dos valores obtidos ao aplicar uma função a cada elemento de um contêiner.
 * @tparam Container Tipo do contêiner iterável (ex.: std::vector, std::deque).
 * @tparam Func Tipo da função, functor ou lambda que recebe um elemento e retorna um valor numérico.
 * @param[in] container Contêiner somente leitura cujos elementos serão percorridos.
 * @param[in] f Função de projeção aplicada a cada elemento.
 * @return Soma acumulada em ponto flutuante (double) dos resultados de f(elemento).
 */
template <typename Container, typename Func>
double somarPor(const Container& container, Func f) {
    double total = 0.0;
    for (const auto& elemento : container) total += f(elemento);
    return total;
}

#endif