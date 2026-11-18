/**
 * @file Carrinho.cpp
 * @brief Implementação das regras de negócio, cálculo polimórfico e descontos da classe Carrinho.
 */

#include "Carrinho.h"
#include "Algoritmos.h"
#include <algorithm>
#include <map>

Carrinho::Carrinho() : sessaoSelecionada(nullptr) {}

void Carrinho::definirSessao(Sessao* sessao) {
    sessaoSelecionada = sessao;
}

Sessao* Carrinho::getSessao() const {
    return sessaoSelecionada;
}

void Carrinho::adicionarIngresso(std::unique_ptr<Ingresso> ingresso) {
    ingressos.push_back(std::move(ingresso));
}

void Carrinho::limparIngressos() {
    ingressos.clear();
}

const std::vector<std::unique_ptr<Ingresso>>& Carrinho::getIngressos() const {
    return ingressos;
}

int Carrinho::getQuantidadeIngressos() const {
    return static_cast<int>(ingressos.size());
}

/** @details Associa cada ponteiro de Assento ao ingresso de mesmo índice caso as quantidades coincidam. */
bool Carrinho::vincularAssentosAosIngressos(const std::vector<Assento*>& assentosSelecionados) {
    if (assentosSelecionados.size() != ingressos.size()) {
        return false;
    }
    for (size_t i = 0; i < ingressos.size(); ++i) {
        ingressos[i]->vincularAssento(assentosSelecionados[i]);
    }
    return true;
}

void Carrinho::cadastrarCombo(const Combo& combo) {
    combosDisponiveis.push_back(combo);
}

const std::vector<Combo>& Carrinho::getCombos() const {
    return combosDisponiveis;
}

Carrinho& Carrinho::operator+=(const ItemBomboniere& item) {
    adicionarItemBomboniere(item);
    return *this;
}

void Carrinho::adicionarItemBomboniere(const ItemBomboniere& item) {
    itensBomboniere.push_back(item);
}

/** @details Localiza a primeira ocorrência por ID com std::find_if e remove apenas uma unidade. */
void Carrinho::removerItemBombonierePorId(int idProduto) {
    auto it = std::find_if(itensBomboniere.begin(), itensBomboniere.end(),
                           [idProduto](const ItemBomboniere& item) {
                               return item.getId() == idProduto;
                           });
    if (it != itensBomboniere.end()) {
        itensBomboniere.erase(it);
    }
}

int Carrinho::contarQuantidadeProduto(int idProduto) const {
    int count = 0;
    for (const auto& item : itensBomboniere) {
        if (item.getId() == idProduto) {
            count++;
        }
    }
    return count;
}

const std::vector<ItemBomboniere>& Carrinho::getItensBomboniere() const {
    return itensBomboniere;
}

bool Carrinho::estaVazio() const {
    return ingressos.empty() && itensBomboniere.empty();
}

/** @details Soma o retorno polimórfico de calcularPreco() via função template somarPor(). */
double Carrinho::calcularSubtotalIngressos() const {
    return somarPor(ingressos, [](const std::unique_ptr<Ingresso>& ing) { return ing->calcularPreco(); });
}

/** @details Soma os preços cheios de prateleira via somarPor() sem aplicar combos. */
double Carrinho::calcularSubtotalBomboniere() const {
    return somarPor(itensBomboniere, [](const ItemBomboniere& item) { return item.getPreco(); });
}

/** @details Ordena combos por maior quantidade de itens e aplica cada regra repetidamente enquanto houver saldo. */
std::vector<const Combo*> Carrinho::combosAplicados() const {
    std::map<int, int> disponiveis;
    for (const auto& item : itensBomboniere) {
        disponiveis[item.getId()]++;
    }

    // Combos maiores primeiro (ponteiros: não copia os combos).
    std::vector<const Combo*> ordenados;
    for (const auto& combo : combosDisponiveis) ordenados.push_back(&combo);
    std::stable_sort(ordenados.begin(), ordenados.end(), [](const Combo* a, const Combo* b) {
        return a->getIdsProdutos().size() > b->getIdsProdutos().size();
    });

    std::vector<const Combo*> aplicados;
    for (const Combo* combo : ordenados) {
        if (combo->getIdsProdutos().empty()) continue;

        std::map<int, int> necessario; // Calculado uma vez por combo
        for (int id : combo->getIdsProdutos()) necessario[id]++;

        for (;;) {
            bool formado = true;
            for (const auto& par : necessario) {
                if (disponiveis[par.first] < par.second) { formado = false; break; }
            }
            if (!formado) break;

            for (const auto& par : necessario) disponiveis[par.first] -= par.second;
            aplicados.push_back(combo);
        }
    }
    return aplicados;
}

/** @details Preenche combosAplicadosOut com os nomes dos combos formados e soma seus descontos via somarPor(). */
double Carrinho::calcularDescontoCombos(std::vector<std::string>& combosAplicadosOut) const {
    combosAplicadosOut.clear();
    const std::vector<const Combo*> aplicados = combosAplicados();
    for (const Combo* combo : aplicados) {
        combosAplicadosOut.push_back(combo->getNome());
    }
    return somarPor(aplicados, [](const Combo* combo) { return combo->getDesconto(); });
}

/** @details Reúne itens e combos em um vetor polimórfico const Produto* somando getValorNoCarrinho(). */
double Carrinho::calcularTotalFinal() const {
    // Polimorfismo: itens somam e combos aplicados subtraem, via Produto::getValorNoCarrinho() virtual.
    std::vector<const Produto*> linhas;
    for (const auto& item : itensBomboniere) linhas.push_back(&item);
    for (const Combo* combo : combosAplicados()) linhas.push_back(combo);
    const double bomboniere = somarPor(linhas, [](const Produto* p) { return p->getValorNoCarrinho(); });

    // Nunca devolve valor negativo, mesmo com combos mal configurados.
    return std::max(0.0, calcularSubtotalIngressos() + bomboniere);
}

/** @details Confirma na sessão os assentos reservados e em seguida limpa o carrinho. */
void Carrinho::finalizarCompra() {
    if (sessaoSelecionada != nullptr) {
        sessaoSelecionada->getSala().confirmarAssentosReservados();
    }
    limparTudo();
}

/** @details Libera assentos ainda RESERVADOS antes de limpar sessão, ingressos e itens da bomboniere. */
void Carrinho::limparTudo() {
    // liberar() só atua em assentos RESERVADOS; os OCUPADOS (compra concluída) ficam intactos.
    for (const auto& ing : ingressos) {
        if (Assento* a = ing->getAssento()) {
            a->liberar();
        }
    }
    sessaoSelecionada = nullptr;
    ingressos.clear();
    itensBomboniere.clear();
}