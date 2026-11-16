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
 * Console de administracao (terminal) para cadastrar filmes, sessoes e produtos
 * enquanto o totem esta rodando.
 *
 * Toda a leitura do teclado acontece em uma thread propria; qualquer acesso aos
 * repositorios e aos widgets e despachado para a thread da GUI pela fila de eventos do Qt.
 * A thread nunca guarda ponteiro cru para a janela: ela so enxerga um estado
 * compartilhado (shared_ptr) que e invalidado no destrutor, entao fechar a janela
 * enquanto o console espera uma linha e seguro.
 */
class AdminConsole {
public:
    /** Notificacoes para a camada visual atualizar as telas (sempre chamadas na thread da GUI). */
    struct Ganchos {
        std::function<void()> aoMudarCatalogo;   // filme ou sessao novos
        std::function<void()> aoMudarBomboniere; // produto novo
    };

    AdminConsole(RepositorioGenerico<Filme>& filmes,
                 RepositorioGenerico<Sessao>& sessoes,
                 RepositorioGenerico<ItemBomboniere>& bomboniere,
                 Ganchos ganchos);
    ~AdminConsole();

    // Possui estado compartilhado com a thread: nao copiavel (Regra dos Tres).
    AdminConsole(const AdminConsole&) = delete;
    AdminConsole& operator=(const AdminConsole&) = delete;

    /** Inicia a thread de leitura do terminal. Chamar uma unica vez, na thread da GUI. */
    void iniciar();

private:
    std::shared_ptr<EstadoAdmin> estado;
};

#endif
