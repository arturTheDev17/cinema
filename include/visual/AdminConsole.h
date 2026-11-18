/**
 * @file AdminConsole.h
 * @brief Declaração da classe AdminConsole para administração em tempo real via terminal.
 */

#ifndef ADMIN_CONSOLE_H
#define ADMIN_CONSOLE_H

#include <functional>
#include <memory>
#include "RepositorioGenerico.h"
#include "Filme.h"
#include "Sessao.h"
#include "ItemBomboniere.h"

struct EstadoAdmin; // definido em AdminConsole.cpp

/**
 * @class AdminConsole
 * @brief Console de administração (terminal) para cadastrar filmes, sessões e produtos enquanto o totem roda.
 *
 * Toda a leitura do teclado acontece em uma thread própria; qualquer acesso aos
 * repositórios e aos widgets é despachado para a thread da GUI pela fila de eventos do Qt.
 * A thread nunca guarda ponteiro cru para a janela: ela só enxerga um estado
 * compartilhado (shared_ptr) que é invalidado no destrutor, então fechar a janela
 * enquanto o console espera uma linha é seguro.
 *
 * @note Possui estado compartilhado com a thread: não copiável (Regra dos Três).
 */
class AdminConsole {
public:
    /**
     * @struct Ganchos
     * @brief Notificações para a camada visual atualizar as telas (sempre chamadas na thread da GUI).
     */
    struct Ganchos {
        std::function<void()> aoMudarCatalogo;   ///< Callback para filme ou sessão novos.
        std::function<void()> aoMudarBomboniere; ///< Callback para produto novo/atualizado/removido.
    };

    /**
     * @brief Constrói o controlador do console administrativo.
     * @param[in,out] filmes Repositório central de filmes.
     * @param[in,out] sessoes Repositório central de sessões.
     * @param[in,out] bomboniere Repositório central da bomboniere.
     * @param[in] ganchos Estrutura de callbacks para atualização da interface gráfica.
     */
    AdminConsole(RepositorioGenerico<Filme>& filmes,
                 RepositorioGenerico<Sessao>& sessoes,
                 RepositorioGenerico<ItemBomboniere>& bomboniere,
                 Ganchos ganchos);

    /**
     * @brief Invalida o estado compartilhado para encerrar a comunicação com segurança.
     */
    ~AdminConsole();

    /** @brief Construtor de cópia proibido (possui estado compartilhado com thread). */
    AdminConsole(const AdminConsole&) = delete;

    /** @brief Operador de atribuição por cópia proibido. */
    AdminConsole& operator=(const AdminConsole&) = delete;

    /**
     * @brief Inicia a thread de leitura do terminal.
     * @note Chamar uma única vez, na thread da GUI.
     */
    void iniciar();

private:
    std::shared_ptr<EstadoAdmin> estado; ///< Estado compartilhado entre a GUI e a thread do console.
};

#endif