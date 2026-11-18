/**
 * @file Tela1SelecaoFilmeWidget.cpp
 * @brief Implementação da tela de seleção de filmes, sessões e ingressos Tela1SelecaoFilmeWidget.
 */

#include "Estilo.h"
#include "Tela1SelecaoFilmeWidget.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QPushButton>
#include <QFrame>
#include <memory>
#include "Formatacao.h"

/** @details Monta grade rolável de filmes, lista de sessões, contadores de ingressos e barra de navegação. */
Tela1SelecaoFilmeWidget::Tela1SelecaoFilmeWidget(RepositorioGenerico<Filme>& filmes,
                                               RepositorioGenerico<Sessao>& sessoes,
                                               Carrinho& carrinho,
                                               QWidget* parent)
    : QWidget(parent), repoFilmes(filmes), repoSessoes(sessoes), carrinho(carrinho), filmeSelecionadoIndex(-1),
      qtdInteiraVal(0), qtdMeiaVal(0), colunasAtuais(0) {

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* colunasLayout = new QHBoxLayout();

    QGroupBox* grpFilmes = new QGroupBox("Filmes em Cartaz", this);
    QVBoxLayout* layFilmesGrp = new QVBoxLayout(grpFilmes);
    
    scrollFilmes = new QScrollArea(this);
    scrollFilmes->setWidgetResizable(true);
    scrollFilmes->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // A grade se adapta; nunca rola de lado
    scrollFilmes->viewport()->installEventFilter(this);                 // Reorganiza quando a área visível muda
    containerFilmesCards = new QWidget(this);
    QGridLayout* gridCardsContainer = new QGridLayout(containerFilmesCards);
    gridCardsContainer->setAlignment(Qt::AlignTop); // Sem AlignLeft: a grade ocupa toda a largura
    gridCardsContainer->setHorizontalSpacing(15);
    gridCardsContainer->setVerticalSpacing(15);
    scrollFilmes->setWidget(containerFilmesCards);

    lblDetalhesFilme = new QLabel("Selecione um filme acima.", this);
    lblDetalhesFilme->setWordWrap(true);
    lblDetalhesFilme->setStyleSheet("color: " COR_TEXTO "; font-weight: bold; padding: 4px;");

    layFilmesGrp->addWidget(scrollFilmes, 1);
    layFilmesGrp->addWidget(lblDetalhesFilme);

    QVBoxLayout* colunaDireitaLayout = new QVBoxLayout();

    QGroupBox* grpSessoes = new QGroupBox("Sessões", this);
    QVBoxLayout* laySessoes = new QVBoxLayout(grpSessoes);
    listaSessoes = new QListWidget(this);
    listaSessoes->setStyleSheet("font-size: 16px;");

    laySessoes->addWidget(listaSessoes);
    colunaDireitaLayout->addWidget(grpSessoes, 2);

    QGroupBox* grpIngressos = new QGroupBox("Quantidade de Ingressos", this);
    QVBoxLayout* layIngressos = new QVBoxLayout(grpIngressos);

    QHBoxLayout* layoutInteira = new QHBoxLayout();
    QLabel* lblInteira = new QLabel("Ingresso Inteira", this);
    lblInteira->setStyleSheet("font-weight: normal; font-size: 16px;");
    
    QHBoxLayout* layoutMeia = new QHBoxLayout();
    QLabel* lblMeia = new QLabel("Ingresso Meia-Entrada (50%)", this);
    lblMeia->setStyleSheet("font-weight: normal; font-size: 16px;");

    btnMenosInteira = new QPushButton("-", this);
    btnMenosInteira->setFixedSize(35, 30);
    btnMenosInteira->setStyleSheet("background-color: " COR_ERRO "; color: white; font-weight: bold;");
    btnMenosInteira->setCursor(Qt::PointingHandCursor);

    lblQtdInteira = new QLabel("0", this);
    lblQtdInteira->setAlignment(Qt::AlignCenter);
    lblQtdInteira->setFixedWidth(30);
    lblQtdInteira->setStyleSheet("font-weight: bold; font-size: 14px; color: " COR_DESTAQUE ";");

    btnMaisInteira = new QPushButton("+", this);
    btnMaisInteira->setFixedSize(35, 30);
    btnMaisInteira->setStyleSheet("background-color: " COR_SUCESSO "; color: white; font-weight: bold;");
    btnMaisInteira->setCursor(Qt::PointingHandCursor);

    layoutInteira->addWidget(lblInteira, 1);
    layoutInteira->addWidget(btnMenosInteira);
    layoutInteira->addWidget(lblQtdInteira);
    layoutInteira->addWidget(btnMaisInteira);
    layIngressos->addLayout(layoutInteira);

    btnMenosMeia = new QPushButton("-", this);
    btnMenosMeia->setFixedSize(35, 30);
    btnMenosMeia->setStyleSheet("background-color: " COR_ERRO "; color: white; font-weight: bold;");
    btnMenosMeia->setCursor(Qt::PointingHandCursor);

    lblQtdMeia = new QLabel("0", this);
    lblQtdMeia->setAlignment(Qt::AlignCenter);
    lblQtdMeia->setFixedWidth(30);
    lblQtdMeia->setStyleSheet("font-weight: bold; font-size: 14px; color: " COR_DESTAQUE ";");

    btnMaisMeia = new QPushButton("+", this);
    btnMaisMeia->setFixedSize(35, 30);
    btnMaisMeia->setStyleSheet("background-color: " COR_SUCESSO "; color: white; font-weight: bold;");
    btnMaisMeia->setCursor(Qt::PointingHandCursor);

    layoutMeia->addWidget(lblMeia, 1);
    layoutMeia->addWidget(btnMenosMeia);
    layoutMeia->addWidget(lblQtdMeia);
    layoutMeia->addWidget(btnMaisMeia);
    layIngressos->addLayout(layoutMeia);

    colunaDireitaLayout->addWidget(grpIngressos, 1);

    colunasLayout->addWidget(grpFilmes, 1);
    colunasLayout->addLayout(colunaDireitaLayout, 1);
    mainLayout->addLayout(colunasLayout, 4);

    btnProsseguir = new QPushButton("Prosseguir para Escolha de Assentos", this);
    btnProsseguir->setObjectName("btnProsseguir");
    btnProsseguir->setMinimumHeight(45);
    btnProsseguir->setEnabled(false);
    btnProsseguir->setCursor(Qt::PointingHandCursor);

    btnVoltar = new QPushButton("Voltar", this);
    btnVoltar->setMinimumWidth(120);
    btnVoltar->setMinimumHeight(45);
    btnVoltar->setCursor(Qt::PointingHandCursor);
    
    QHBoxLayout* navLayout = new QHBoxLayout();
    navLayout->addWidget(btnVoltar);
    navLayout->addWidget(btnProsseguir, 2);
    mainLayout->addLayout(navLayout);

    reconstruirCardsFilmes();

    connect(listaSessoes, &QListWidget::currentRowChanged, this, &Tela1SelecaoFilmeWidget::onSessaoSelecionada);
    
    connect(btnMaisInteira, &QPushButton::clicked, this, [this]() {
        qtdInteiraVal++;
        lblQtdInteira->setText(QString::number(qtdInteiraVal));
        onQuantidadeAlterada();
    });
    connect(btnMenosInteira, &QPushButton::clicked, this, [this]() {
        if (qtdInteiraVal > 0) {
            qtdInteiraVal--;
            lblQtdInteira->setText(QString::number(qtdInteiraVal));
            onQuantidadeAlterada();
        }
    });

    connect(btnMaisMeia, &QPushButton::clicked, this, [this]() {
        qtdMeiaVal++;
        lblQtdMeia->setText(QString::number(qtdMeiaVal));
        onQuantidadeAlterada();
    });
    connect(btnMenosMeia, &QPushButton::clicked, this, [this]() {
        if (qtdMeiaVal > 0) {
            qtdMeiaVal--;
            lblQtdMeia->setText(QString::number(qtdMeiaVal));
            onQuantidadeAlterada();
        }
    });

    connect(btnProsseguir, &QPushButton::clicked, this, &Tela1SelecaoFilmeWidget::onProsseguirClicado);
    connect(btnVoltar, &QPushButton::clicked, this, &Tela1SelecaoFilmeWidget::voltarParaInicio);
}

/** @details Destrói os cards anteriores via deleteLater() e redistribui as colunas usadas no QGridLayout. */
void Tela1SelecaoFilmeWidget::reconstruirCardsFilmes() {
    QLayoutItem* child;
    QGridLayout* gridLayout = qobject_cast<QGridLayout*>(containerFilmesCards->layout());
    
    if (!gridLayout) {
        gridLayout = new QGridLayout(containerFilmesCards);
        containerFilmesCards->setLayout(gridLayout);
    }

    // deleteLater: o botão que emitiu o clicked() pode estar sendo destruído por aqui.
    while ((child = gridLayout->takeAt(0)) != nullptr) {
        if (QWidget* w = child->widget()) {
            w->hide();
            w->deleteLater();
        }
        delete child;
    }

    const auto& filmes = repoFilmes.listarTodos();
    if (filmes.empty()) return;

    const int colunasMax = calcularColunas();
    colunasAtuais = colunasMax;

    int col = 0, row = 0;
    for (size_t i = 0; i < filmes.size(); ++i) {
        const auto& filme = filmes[i];
        QPushButton* btnCard = new QPushButton(this);
        btnCard->setFixedSize(kLarguraCard, kAlturaCard);
        btnCard->setCursor(Qt::PointingHandCursor);

        // O conteúdo do card são QLabels com quebra de linha: títulos longos
        // (ex.: "Avatar: O Caminho da Água") ocupam várias linhas em vez de serem cortados.
        QVBoxLayout* layCard = new QVBoxLayout(btnCard);
        layCard->setContentsMargins(8, 10, 8, 10);
        layCard->setSpacing(6);

        auto criarTexto = [btnCard](const QString& texto, int tamanhoPx, bool negrito) {
            QLabel* lbl = new QLabel(texto, btnCard);
            lbl->setAlignment(Qt::AlignCenter);
            lbl->setWordWrap(true);
            lbl->setAttribute(Qt::WA_TransparentForMouseEvents); // O clique vai para o botão
            lbl->setStyleSheet(QString("font-size: %1px; font-weight: %2;")
                                   .arg(tamanhoPx).arg(negrito ? "bold" : "normal"));
            return lbl;
        };
        layCard->addStretch(1);
        layCard->addWidget(criarTexto("🎬", 28, false));
        layCard->addWidget(criarTexto(QString::fromStdString(filme.getTitulo()), 15, true));
        layCard->addWidget(criarTexto(QString::fromStdString(filme.getGenero()), 13, false));
        layCard->addWidget(criarTexto(QString("%1 min").arg(filme.getDuracao()), 13, false));
        layCard->addStretch(1);

        const QString estiloLabels = "QPushButton QLabel { background: transparent; border: none; color: " COR_TEXTO "; } ";
        QString estiloCard;
        if (static_cast<int>(i) == filmeSelecionadoIndex) {
            estiloCard = "QPushButton { background-color: " COR_FUNDO "; border: 3px solid " COR_DESTAQUE "; border-radius: 10px; } "
                         "QPushButton QLabel { background: transparent; border: none; color: " COR_DESTAQUE "; }";
        } else {
            estiloCard = "QPushButton { background-color: " COR_BORDA "; border: 2px solid " COR_FUNDO "; border-radius: 10px; } "
                         + estiloLabels +
                         "QPushButton:hover { border-color: " COR_DESTAQUE "; } "
                         "QPushButton:hover QLabel { color: " COR_DESTAQUE "; }";
        }
        btnCard->setStyleSheet(estiloCard);

        int idx = static_cast<int>(i);
        connect(btnCard, &QPushButton::clicked, this, [this, idx]() { onFilmeCardClicado(idx); });

        gridLayout->addWidget(btnCard, row, col, Qt::AlignCenter);
        col++;
        if (col >= colunasMax) { col = 0; row++; }
    }

    // O QGridLayout nunca "encolhe": colunas de reconstruções anteriores continuam existindo.
    // Zera todas e só distribui por igual as colunas realmente usadas agora.
    for (int c = 0; c < gridLayout->columnCount(); ++c) {
        gridLayout->setColumnStretch(c, c < colunasMax ? 1 : 0);
        gridLayout->setColumnMinimumWidth(c, 0);
    }
}

/** @details Calcula quantas colunas de kLarguraCard cabem na largura útil do viewport. */
int Tela1SelecaoFilmeWidget::calcularColunas() const {
    int espaco = 15;
    int margens = 22;
    if (auto* grid = qobject_cast<QGridLayout*>(containerFilmesCards->layout())) {
        if (grid->horizontalSpacing() >= 0) espaco = grid->horizontalSpacing();
        const QMargins m = grid->contentsMargins();
        margens = m.left() + m.right();
    }

    // Largura realmente disponível: área visível menos margens e uma folga para a barra vertical.
    const int util = scrollFilmes->viewport()->width() - margens - 12;
    int colunas = (util + espaco) / (kLarguraCard + espaco);
    if (colunas < 1) colunas = 1;

    const int totalFilmes = static_cast<int>(repoFilmes.tamanho());
    if (totalFilmes > 0 && colunas > totalFilmes) colunas = totalFilmes;
    return colunas;
}

/** @details Reconstrói a grade caso o redimensionamento do viewport altere a quantidade de colunas. */
bool Tela1SelecaoFilmeWidget::eventFilter(QObject* alvo, QEvent* evento) {
    if (alvo == scrollFilmes->viewport() && evento->type() == QEvent::Resize
        && calcularColunas() != colunasAtuais) {
        reconstruirCardsFilmes();
    }
    return QWidget::eventFilter(alvo, evento);
}

/** @details Reorganiza os cards apenas quando o número de colunas muda. */
void Tela1SelecaoFilmeWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (calcularColunas() != colunasAtuais) {
        reconstruirCardsFilmes();
    }
}

/** @details Atualiza a grade ao exibir a tela se o número ideal de colunas mudou. */
void Tela1SelecaoFilmeWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    if (calcularColunas() != colunasAtuais) {
        reconstruirCardsFilmes();
    }
}

/** @details Atualiza os cards e a lista de sessões preservando a seleção atual. */
void Tela1SelecaoFilmeWidget::atualizarCatalogo() {
    reconstruirCardsFilmes();
    reconstruirListaSessoes(); // Mantém a sessão já escolhida, se houver
}

/** @details Limpa seleção de filme, sessões, detalhes e zera os contadores de ingressos. */
void Tela1SelecaoFilmeWidget::resetar() {
    filmeSelecionadoIndex = -1;
    reconstruirCardsFilmes();
    listaSessoes->clear();
    indicesSessoesFiltradas.clear();
    lblDetalhesFilme->setText("Selecione um filme acima.");
    
    qtdInteiraVal = 0;
    qtdMeiaVal = 0;
    if (lblQtdInteira) lblQtdInteira->setText("0");
    if (lblQtdMeia) lblQtdMeia->setText("0");
    
    btnProsseguir->setEnabled(false);
}

/** @details Marca o filme clicado, exibe seus metadados e popula as sessões correspondentes. */
void Tela1SelecaoFilmeWidget::onFilmeCardClicado(int indexFilme) {
    filmeSelecionadoIndex = indexFilme;
    reconstruirCardsFilmes();

    listaSessoes->clear();
    indicesSessoesFiltradas.clear();
    btnProsseguir->setEnabled(false);

    if (indexFilme < 0 || static_cast<size_t>(indexFilme) >= repoFilmes.tamanho()) return;

    const Filme& filme = repoFilmes.obter(static_cast<size_t>(indexFilme));
    lblDetalhesFilme->setText(QString("Filme: %1 | Gênero: %2 | Duração: %3 min | Classificação: %4 anos")
                                .arg(QString::fromStdString(filme.getTitulo()))
                                .arg(QString::fromStdString(filme.getGenero()))
                                .arg(filme.getDuracao())
                                .arg(filme.getClassificacao()));

    reconstruirListaSessoes();
}

/** @details Filtra repoSessoes pelo ID do filme selecionado e restaura a linha previamente escolhida. */
void Tela1SelecaoFilmeWidget::reconstruirListaSessoes() {
    const int linhaAnterior = listaSessoes->currentRow();
    listaSessoes->clear();
    indicesSessoesFiltradas.clear();

    if (filmeSelecionadoIndex < 0 || static_cast<size_t>(filmeSelecionadoIndex) >= repoFilmes.tamanho()) return;
    const Filme& filme = repoFilmes.obter(static_cast<size_t>(filmeSelecionadoIndex));

    for (size_t i = 0; i < repoSessoes.tamanho(); ++i) {
        const Sessao& s = repoSessoes.obter(i);
        if (s.getFilme().getId() == filme.getId()) {
            indicesSessoesFiltradas.push_back(static_cast<int>(i));
            QString itemTexto = QString("%1 - %2 (Livres: %3)")
                                    .arg(QString::fromStdString(s.getDescricaoCompleta()))
                                    .arg(formatarMoeda(s.getPrecoBase()))
                                    .arg(s.getSala().contarAssentosLivres());
            listaSessoes->addItem(itemTexto);
        }
    }

    // Novas sessões entram no fim da lista, então a linha escolhida continua válida.
    if (linhaAnterior >= 0 && linhaAnterior < listaSessoes->count()) {
        listaSessoes->setCurrentRow(linhaAnterior);
    }
}

void Tela1SelecaoFilmeWidget::onSessaoSelecionada(int) {
    onQuantidadeAlterada();
}

void Tela1SelecaoFilmeWidget::onQuantidadeAlterada() {
    int totalIngressos = qtdInteiraVal + qtdMeiaVal;
    bool sessaoValida = listaSessoes->currentRow() >= 0;
    btnProsseguir->setEnabled(sessaoValida && totalIngressos > 0);
}

/** @details Confere lotação disponível, cria os objetos IngressoInteira e IngressoMeia no carrinho e avança. */
void Tela1SelecaoFilmeWidget::onProsseguirClicado() {
    int rowSessao = listaSessoes->currentRow();
    if (rowSessao < 0 || static_cast<size_t>(rowSessao) >= indicesSessoesFiltradas.size()) return;

    int idxReal = indicesSessoesFiltradas[static_cast<size_t>(rowSessao)];
    Sessao& sessaoEscolhida = repoSessoes.obter(static_cast<size_t>(idxReal));

    int totalIngressos = qtdInteiraVal + qtdMeiaVal;
    if (totalIngressos > sessaoEscolhida.getSala().contarAssentosLivres()) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Aviso");
        msgBox.setText("Não há assentos livres suficientes nesta sessão.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    carrinho.limparTudo();
    carrinho.definirSessao(&sessaoEscolhida);

    double base = sessaoEscolhida.getPrecoBase();

    for (int i = 0; i < qtdInteiraVal; ++i) {
        carrinho.adicionarIngresso(std::make_unique<IngressoInteira>(base));
    }
    
    for (int i = 0; i < qtdMeiaVal; ++i) {
        carrinho.adicionarIngresso(std::make_unique<IngressoMeia>(base));
        
    }
    
    emit avancarParaAssentos();
}