#ifndef DADOS_INICIAIS_H
#define DADOS_INICIAIS_H

#include "RepositorioGenerico.h"
#include "Filme.h"
#include "Sessao.h"
#include "ItemBomboniere.h"
#include "Carrinho.h"

/**
 * Popula os repositorios e as regras de combo com os dados de demonstracao do totem.
 * Fica fora da MainWindow para separar dados de dominio da camada visual.
 */
void carregarDadosIniciais(RepositorioGenerico<Filme>& filmes,
                           RepositorioGenerico<Sessao>& sessoes,
                           RepositorioGenerico<ItemBomboniere>& bomboniere,
                           Carrinho& carrinho);

#endif
