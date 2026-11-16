#include "Carrinho.h"
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

bool Carrinho::vincularAssentosAosIngressos(const std::vector<Assento*>& assentosSelecionados) {
    if (assentosSelecionados.size() != ingressos.size()) {
        return false;
    }
    for (size_t i = 0; i < ingressos.size(); ++i) {
        ingressos[i]->vincularAssento(assentosSelecionados[i]);
    }
    return true;
}

void Carrinho::cadastrarRegraCombo(const RegraCombo& regra) {
    regrasCombosDisponiveis.push_back(regra);
}

void Carrinho::adicionarItemBomboniere(const ItemBomboniere& item) {
    itensBomboniere.push_back(item);
}

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

double Carrinho::calcularSubtotalIngressos() const {
    double total = 0.0;
    for (const auto& ing : ingressos) {
        total += ing->calcularPreco();
    }
    return total;
}

double Carrinho::calcularSubtotalBomboniere() const {
    double total = 0.0;
    for (const auto& item : itensBomboniere) {
        total += item.getPreco();
    }
    return total;
}

double Carrinho::calcularDescontoCombos(std::vector<std::string>& combosAplicadosOut) const {
    combosAplicadosOut.clear();
    double descontoTotal = 0.0;

    std::map<int, int> contagemItens;
    for (const auto& item : itensBomboniere) {
        contagemItens[item.getId()]++;
    }

    auto regrasOrdenadas = regrasCombosDisponiveis;
    std::sort(regrasOrdenadas.begin(), regrasOrdenadas.end(), [](const RegraCombo& a, const RegraCombo& b) {
        return a.idsProdutosNecessarios.size() > b.idsProdutosNecessarios.size();
    });

    for (const auto& regra : regrasOrdenadas) {
        if (regra.idsProdutosNecessarios.empty()) continue;

        bool comboFormado = true;
        while (comboFormado) {
            std::map<int, int> consumoTemp;
            for (int idNec : regra.idsProdutosNecessarios) {
                consumoTemp[idNec]++;
            }

            for (const auto& par : consumoTemp) {
                if (contagemItens[par.first] < par.second) {
                    comboFormado = false;
                    break;
                }
            }

            if (comboFormado) {
                for (const auto& par : consumoTemp) {
                    contagemItens[par.first] -= par.second;
                }
                descontoTotal += regra.valorDesconto;
                combosAplicadosOut.push_back(regra.nomeCombo);
            }
        }
    }

    return descontoTotal;
}

double Carrinho::calcularTotalFinal() const {
    std::vector<std::string> dummy;
    double subIng = calcularSubtotalIngressos();
    double subBomb = calcularSubtotalBomboniere();
    double desc = calcularDescontoCombos(dummy);
    return (subIng + subBomb) - desc;
}

void Carrinho::finalizarCompra() {
    if (sessaoSelecionada != nullptr) {
        sessaoSelecionada->getSala().confirmarAssentosReservados();
    }
    limparTudo();
}

void Carrinho::limparTudo() {
    sessaoSelecionada = nullptr;
    ingressos.clear();
    itensBomboniere.clear();
}
