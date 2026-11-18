# CinemaMecha

**C++17 · Qt 5/6 · CMake**

Totem de autoatendimento de cinema com interface gráfica Qt. O usuário seleciona filme, sessão, assentos e itens de bomboniere; o sistema calcula combos e finaliza a compra. Um console de administração paralelo permite cadastrar filmes, sessões e produtos em tempo real, sem interromper o fluxo de compra.

## Visão geral

O sistema simula o totem de um multiplex: quatro telas guiam o usuário do catálogo de filmes até a confirmação do pagamento. Os dados (filmes, sessões, produtos) vivem em memória e são pré-carregados pela função `carregarDadosIniciais`. Um console de texto, executado em thread separada, expõe operações administrativas sem bloquear a GUI.

## Funcionalidades

- **Tela inicial** — ponto de entrada; inicia ou retoma uma compra.
- **Tela 1 · Seleção de filme e sessão** — grade de cards de filmes, lista de sessões filtrada por filme, seleção de quantidade de ingressos (inteira e meia-entrada), ordenação automática por horário.
- **Tela 2 · Mapa de assentos** — grade interativa da sala, bloqueio visual de assentos ocupados, reserva temporária durante a compra.
- **Tela 3 · Bomboniere** — catálogo com imagem e preço, controle de quantidade por produto, aplicação automática de combos com desconto.
- **Tela 4 · Pagamento** — resumo do pedido (ingressos + bomboniere + descontos), total final, confirmação que marca assentos como ocupados.
- **Cancelamento por inatividade** — 5 minutos sem interação liberam os assentos reservados e voltam à tela inicial.
- **Console de administração** — cadastro de filmes, sessões e produtos; atualização e remoção de preços; listagem geral; tudo despachado para a thread da GUI via fila de eventos Qt.

## Arquitetura e estrutura de diretórios

O projeto separa domínio e interface: `include/classes` + `src/classes` contêm toda a lógica de negócio sem dependência de Qt; `include/visual` + `src/visual` contêm os widgets.

```
cinema/
├── CMakeLists.txt
├── include/
│   ├── classes/
│   │   ├── Algoritmos.h          # função template somarPor<>
│   │   ├── Assento.h
│   │   ├── Carrinho.h
│   │   ├── Combo.h
│   │   ├── DadosIniciais.h
│   │   ├── Filme.h
│   │   ├── Ingresso.h
│   │   ├── ItemBomboniere.h
│   │   ├── Produto.h
│   │   ├── RepositorioGenerico.h # classe template
│   │   ├── Sala.h
│   │   └── Sessao.h
│   └── visual/
│       ├── AdminConsole.h
│       ├── BarraTituloWidget.h
│       ├── Estilo.h              # paleta de cores (macros)
│       ├── Formatacao.h          # formatarMoeda()
│       ├── MainWindow.h
│       ├── SidebarCarrinhoWidget.h
│       ├── Tela1SelecaoFilmeWidget.h
│       ├── Tela2AssentosWidget.h
│       ├── Tela3BomboniereWidget.h
│       └── Tela4PagamentoWidget.h
├── src/
│   ├── classes/          # implementações .cpp do domínio
│   ├── visual/           # implementações .cpp dos widgets
│   ├── img/              # recursos de imagem (QRC)
│   ├── main.cpp
│   └── recursos.qrc
```

**Hierarquias de herança:**

- `Produto` ← `ItemBomboniere`, `Combo`
- `Ingresso` ← `IngressoInteira`, `IngressoMeia`

## Pré-requisitos

| Dependência | Versão mínima |
| --- | --- |
| Compilador C++ | GCC 10, Clang 12 ou MSVC 2019 (suporte a C++17) |
| CMake | 3.16 |
| Qt | 5.15 ou 6.x (apenas módulo `Widgets`) |

### Instalação e build

```bash
# 1. Clone o repositório
git clone https://github.com/<seu-usuario>/CinemaMecha.git
cd CinemaMecha

# 2. Configure (substitua o caminho do Qt se necessário)
cmake -S cinema -B build -DCMAKE_BUILD_TYPE=Release

# 3. Compile
cmake --build build --parallel

# 4. Execute
./build/CinemaMecha          # Linux/macOS
build\CinemaMecha.exe        # Windows
```

> **Windows:** o CMake detecta automaticamente o `windeployqt` e copia as DLLs do Qt para a pasta de saída após o build.

## Como usar

### Fluxo de compra

1. Na tela inicial, toque em **Iniciar**.
2. Escolha um filme → selecione uma sessão → defina a quantidade de ingressos (Inteira / Meia-Entrada) → **Prosseguir**.
3. No mapa de assentos, selecione um assento por ingresso → **Avançar**.
4. Na bomboniere, adicione produtos opcionalmente; combos são calculados automaticamente → **Avançar**.
5. Confirme o pedido na tela de pagamento → **Confirmar Pagamento**.

### Console de administração

O console é exibido no terminal a partir do qual a aplicação foi iniciada. Comandos disponíveis:

| Tecla | Ação |
| --- | --- |
| `1` | Adicionar filme |
| `2` | Adicionar produto de bomboniere |
| `3` | Listar filmes, sessões e produtos |
| `4` | Adicionar sessão |
| `5` | Atualizar preço de produto |
| `6` | Remover produto |

As alterações ficam visíveis na GUI imediatamente, sem reiniciar a aplicação.

## Conceitos de POO aplicados

| Conceito | Onde | Evidência |
| --- | --- | --- |
| Abstração | `Produto`, `Ingresso` | Classes base com método puro `getValorNoCarrinho()` / `calcularPreco()` |
| Encapsulamento | `Sala`, `Assento`, `Carrinho` | Atributos `private`; setters com validação; cópia deletada em `Carrinho` |
| Herança | `ItemBomboniere : Produto`, `Combo : Produto`, `IngressoInteira : Ingresso`, `IngressoMeia : Ingresso` | Duas hierarquias independentes |
| Polimorfismo dinâmico | `Carrinho::calcularTotalFinal()` | Opera sobre `vector<const Produto*>` heterogêneo via `getValorNoCarrinho()` virtual |
| Polimorfismo estático | `RepositorioGenerico::buscar()`, `Sessao::operator<`, `Carrinho::operator+=` | Sobrecarga de função por tipo de argumento e de operadores |
| Programação genérica | `RepositorioGenerico<T>`, `somarPor<Container, Func>` | Classe e função template com dois parâmetros de tipo distintos |
| Regra do Zero / Três / Cinco | `Sala` (zero), `AdminConsole` (três), `Carrinho` (cinco) | Estratégia documentada por caso |
| LSP | Contrato de `Produto::getValorNoCarrinho()` | Documentado no header; `Combo` retorna `−desconto` sem violar o contrato |

## Licença e créditos

Projeto acadêmico — Programação III, Engenharia Mecatrônica.

Desenvolvido com [Qt](https://www.qt.io/) (LGPL) e C++17.
