/**
 * @file DadosIniciais.h
 * @brief Declaração da função auxiliar de carga inicial dos dados de demonstração do sistema.
 */

#ifndef DADOS_INICIAIS_H
#define DADOS_INICIAIS_H

#include "RepositorioGenerico.h"
#include "Filme.h"
#include "Sessao.h"
#include "ItemBomboniere.h"
#include "Carrinho.h"

/**
 * @brief Popula os repositórios e as regras de combo com os dados de demonstração do totem.
 * @param[in,out] filmes Repositório de filmes em cartaz a ser preenchido.
 * @param[in,out] sessoes Repositório de sessões disponíveis a ser preenchido.
 * @param[in,out] bomboniere Repositório de produtos da bomboniere a ser preenchido.
 * @param[in,out] carrinho Carrinho onde os combos promocionais serão cadastrados.
 * @note Mantida fora da MainWindow para separar os dados de domínio da camada visual.
 */
void carregarDadosIniciais(RepositorioGenerico<Filme>& filmes,
                           RepositorioGenerico<Sessao>& sessoes,
                           RepositorioGenerico<ItemBomboniere>& bomboniere,
                           Carrinho& carrinho);

#endif