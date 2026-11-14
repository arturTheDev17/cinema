#include "ItemBomboniere.h"

int ItemBomboniere::proximoId = 1;

ItemBomboniere::ItemBomboniere(const std::string& nome, double preco, const std::string& caminhoImagem)
    : id(proximoId++), nome(nome), preco(preco), caminhoImagem(caminhoImagem) {}

ItemBomboniere::ItemBomboniere(int idEspecifico, const std::string& nome, double preco, const std::string& caminhoImagem)
    : id(idEspecifico), nome(nome), preco(preco), caminhoImagem(caminhoImagem) {
    if (idEspecifico >= proximoId) {
        proximoId = idEspecifico + 1;
    }
}

int ItemBomboniere::getId() const {
    return id;
}

std::string ItemBomboniere::getNome() const {
    return nome;
}

double ItemBomboniere::getPreco() const {
    return preco;
}

std::string ItemBomboniere::getCaminhoImagem() const {
    return caminhoImagem;
}

bool ItemBomboniere::operator==(const ItemBomboniere& outro) const {
    return this->id == outro.id;
}
