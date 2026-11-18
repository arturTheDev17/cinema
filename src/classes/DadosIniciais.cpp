/**
 * @file DadosIniciais.cpp
 * @brief Implementação da carga inicial de filmes, salas, sessões, bomboniere e combos.
 */

#include "DadosIniciais.h"

/** @details Instancia os filmes, salas modelo, 13 sessões, 8 itens de bomboniere e 2 combos promocionais. */
void carregarDadosIniciais(RepositorioGenerico<Filme>& repoFilmes,
                           RepositorioGenerico<Sessao>& repoSessoes,
                           RepositorioGenerico<ItemBomboniere>& repoBomboniere,
                           Carrinho& carrinho) {
    Filme f1("Duna: Parte 2", 166, 14, "Ficção Científica");
    Filme f2("Interestelar", 169, 10, "Ficção / Drama");
    Filme f3("Oppenheimer", 180, 16, "Biografia / Drama");
    Filme f4("Avatar: O Caminho da Água", 192, 12, "Ficção / Aventura");
    Filme f5("A Origem", 148, 12, "Ficção / Ação");
    Filme f6("Matrix", 136, 14, "Ficção / Ação");

    repoFilmes.adicionar(f1);
    repoFilmes.adicionar(f2);
    repoFilmes.adicionar(f3);
    repoFilmes.adicionar(f4);
    repoFilmes.adicionar(f5);
    repoFilmes.adicionar(f6);

    Sala sala1(1, 6, 8, "Padrão");
    Sala sala2(2, 6, 8, "3D");
    Sala sala3(3, 6, 8, "Padrão");
    Sala salaVIP(4, 6, 8, "VIP");

    // Duna
    repoSessoes.adicionar(Sessao(1, f1, sala1, "14:30", TipoAudio::DUBLADO, TecnologiaExibicao::DOIS_D, 32.0));
    repoSessoes.adicionar(Sessao(2, f1, sala2, "18:00", TipoAudio::LEGENDADO, TecnologiaExibicao::TRES_D, 40.0));
    repoSessoes.adicionar(Sessao(3, f1, salaVIP, "21:15", TipoAudio::LEGENDADO, TecnologiaExibicao::DOIS_D, 50.0));

    // Interestelar
    repoSessoes.adicionar(Sessao(4, f2, sala1, "16:00", TipoAudio::LEGENDADO, TecnologiaExibicao::DOIS_D, 30.0));
    repoSessoes.adicionar(Sessao(5, f2, salaVIP, "20:00", TipoAudio::LEGENDADO, TecnologiaExibicao::DOIS_D, 48.0));

    // Oppenheimer
    repoSessoes.adicionar(Sessao(6, f3, sala2, "19:30", TipoAudio::LEGENDADO, TecnologiaExibicao::DOIS_D, 34.0));
    repoSessoes.adicionar(Sessao(7, f3, sala3, "22:00", TipoAudio::DUBLADO, TecnologiaExibicao::DOIS_D, 32.0));

    // Avatar
    repoSessoes.adicionar(Sessao(8, f4, sala2, "15:00", TipoAudio::DUBLADO, TecnologiaExibicao::TRES_D, 40.0));
    repoSessoes.adicionar(Sessao(9, f4, salaVIP, "19:00", TipoAudio::LEGENDADO, TecnologiaExibicao::TRES_D, 55.0));

    // A Origem
    repoSessoes.adicionar(Sessao(10, f5, sala1, "17:00", TipoAudio::LEGENDADO, TecnologiaExibicao::DOIS_D, 30.0));
    repoSessoes.adicionar(Sessao(11, f5, salaVIP, "21:00", TipoAudio::DUBLADO, TecnologiaExibicao::DOIS_D, 48.0));

    // Matrix
    repoSessoes.adicionar(Sessao(12, f6, sala1, "15:00", TipoAudio::LEGENDADO, TecnologiaExibicao::DOIS_D, 30.0));
    repoSessoes.adicionar(Sessao(13, f6, salaVIP, "19:00", TipoAudio::DUBLADO, TecnologiaExibicao::DOIS_D, 48.0));

    repoBomboniere.adicionar(ItemBomboniere(1, "Pipoca Grande", 26.0, ":/img/pipoca.png"));
    repoBomboniere.adicionar(ItemBomboniere(2, "Pipoca Média", 14.0, ":/img/pipoca2.png"));
    repoBomboniere.adicionar(ItemBomboniere(3, "Refrigerante 700ml", 14.0, ":/img/refri.png"));
    repoBomboniere.adicionar(ItemBomboniere(4, "Refrigerante 500ml", 10.0, ":/img/refri.png"));
    repoBomboniere.adicionar(ItemBomboniere(5, "Chocolate Barra", 10.0, ":/img/chocolate.png"));
    repoBomboniere.adicionar(ItemBomboniere(6, "Nachos com Cheddar", 24.0, ":/img/nachos.png"));
    repoBomboniere.adicionar(ItemBomboniere(7, "Água Mineral 500ml", 7.0, ":/img/agua.png"));
    repoBomboniere.adicionar(ItemBomboniere(8, "Bala de Goma", 6.0, ":/img/bala.png"));

    carrinho.cadastrarCombo(Combo("Combo Solo", {1, 3}, 4.0));
    carrinho.cadastrarCombo(Combo("Combo Casal", {1, 1, 3, 3}, 10.0));
}