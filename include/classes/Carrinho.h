#ifndef CARRINHO_H
#define CARRINHO_H

#include "Sessao.h"
#include "Ingresso.h"
#include "ItemBomboniere.h"
#include <vector>
#include <memory>
#include <string>

class Carrinho {
private:
    Sessao* sessaoSelecionada;
    std::vector<std::unique_ptr<Ingresso>> ingressos;
    std::vector<ItemBomboniere> itensBomboniere;
    std::vector<RegraCombo> regrasCombosDisponiveis;

public:
    Carrinho();

    void definirSessao(Sessao* sessao);
    Sessao* getSessao() const;

    void adicionarIngresso(std::unique_ptr<Ingresso> ingresso);
    void limparIngressos();
    const std::vector<std::unique_ptr<Ingresso>>& getIngressos() const;
    int getQuantidadeIngressos() const;
    bool vincularAssentosAosIngressos(const std::vector<Assento*>& assentosSelecionados);

    void cadastrarRegraCombo(const RegraCombo& regra);
    void adicionarItemBomboniere(const ItemBomboniere& item);
    void removerItemBombonierePorId(int idProduto);
    int contarQuantidadeProduto(int idProduto) const;
    const std::vector<ItemBomboniere>& getItensBomboniere() const;

    // true quando nao ha ingressos nem itens de bomboniere (nada a pagar).
    bool estaVazio() const;

    double calcularSubtotalIngressos() const;
    double calcularSubtotalBomboniere() const;
    double calcularDescontoCombos(std::vector<std::string>& combosAplicadosOut) const;
    double calcularTotalFinal() const;

    void finalizarCompra();
    void limparTudo();
};

#endif
