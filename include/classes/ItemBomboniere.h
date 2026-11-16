#ifndef ITEM_BOMBONIERE_H
#define ITEM_BOMBONIERE_H

#include <string>
#include <vector>

class ItemBomboniere {
private:
    int id;
    std::string nome;
    double preco;
    std::string caminhoImagem;
    
    static int proximoId; // Contador estatico

public:
    ItemBomboniere(const std::string& nome, double preco, const std::string& caminhoImagem = "");
    ItemBomboniere(int idEspecifico, const std::string& nome, double preco, const std::string& caminhoImagem = ""); // Para carregar os padroes

    int getId() const;
    std::string getNome() const;
    double getPreco() const;
    std::string getCaminhoImagem() const;

    bool operator==(const ItemBomboniere& outro) const;
};
struct RegraCombo {
    std::string nomeCombo;
    std::vector<int> idsProdutosNecessarios;
    double valorDesconto;
};

#endif
