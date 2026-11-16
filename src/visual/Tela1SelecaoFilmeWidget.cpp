#include "Tela1SelecaoFilmeWidget.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QPushButton>
#include <QFrame>
#include <memory>

Tela1SelecaoFilmeWidget::Tela1SelecaoFilmeWidget(RepositorioGenerico<Filme>& filmes,
                                               RepositorioGenerico<Sessao>& sessoes,
                                               Carrinho& carrinho,
                                               QWidget* parent)
    : QWidget(parent), repoFilmes(filmes), repoSessoes(sessoes), carrinho(carrinho), filmeSelecionadoIndex(-1),
      qtdInteiraVal(0), qtdMeiaVal(0) {

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* colunasLayout = new QHBoxLayout();

    QGroupBox* grpFilmes = new QGroupBox("Filmes em Cartaz", this);
    QVBoxLayout* layFilmesGrp = new QVBoxLayout(grpFilmes);
    
    QScrollArea* scrollFilmes = new QScrollArea(this);
    scrollFilmes->setWidgetResizable(true);
    containerFilmesCards = new QWidget(this);
    QGridLayout* gridCardsContainer = new QGridLayout(containerFilmesCards);
    gridCardsContainer->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    gridCardsContainer->setHorizontalSpacing(15);
    gridCardsContainer->setVerticalSpacing(15);
    scrollFilmes->setWidget(containerFilmesCards);

    lblDetalhesFilme = new QLabel("Selecione um filme acima.", this);
    lblDetalhesFilme->setWordWrap(true);
    lblDetalhesFilme->setStyleSheet("color: #fbfef9; font-weight: bold; padding: 4px;");

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
    btnMenosInteira->setStyleSheet("background-color: #D32222; color: white; font-weight: bold;");
    btnMenosInteira->setCursor(Qt::PointingHandCursor);

    lblQtdInteira = new QLabel("0", this);
    lblQtdInteira->setAlignment(Qt::AlignCenter);
    lblQtdInteira->setFixedWidth(30);
    lblQtdInteira->setStyleSheet("font-weight: bold; font-size: 14px; color: #dec402;");

    btnMaisInteira = new QPushButton("+", this);
    btnMaisInteira->setFixedSize(35, 30);
    btnMaisInteira->setStyleSheet("background-color: #7FBB3A; color: white; font-weight: bold;");
    btnMaisInteira->setCursor(Qt::PointingHandCursor);

    layoutInteira->addWidget(lblInteira, 1);
    layoutInteira->addWidget(btnMenosInteira);
    layoutInteira->addWidget(lblQtdInteira);
    layoutInteira->addWidget(btnMaisInteira);
    layIngressos->addLayout(layoutInteira);

    btnMenosMeia = new QPushButton("-", this);
    btnMenosMeia->setFixedSize(35, 30);
    btnMenosMeia->setStyleSheet("background-color: #D32222; color: white; font-weight: bold;");
    btnMenosMeia->setCursor(Qt::PointingHandCursor);

    lblQtdMeia = new QLabel("0", this);
    lblQtdMeia->setAlignment(Qt::AlignCenter);
    lblQtdMeia->setFixedWidth(30);
    lblQtdMeia->setStyleSheet("font-weight: bold; font-size: 14px; color: #dec402;");

    btnMaisMeia = new QPushButton("+", this);
    btnMaisMeia->setFixedSize(35, 30);
    btnMaisMeia->setStyleSheet("background-color: #7FBB3A; color: white; font-weight: bold;");
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

    btnVoltar = new QPushButton("Voltar ao Início", this);
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

void Tela1SelecaoFilmeWidget::reconstruirCardsFilmes() {
    QLayoutItem* child;
    QGridLayout* gridLayout = qobject_cast<QGridLayout*>(containerFilmesCards->layout());
    
    if (!gridLayout) {
        gridLayout = new QGridLayout(containerFilmesCards);
        containerFilmesCards->setLayout(gridLayout);
    }

    // deleteLater: o botao que emitiu o clicked() pode estar sendo destruido por aqui.
    while ((child = gridLayout->takeAt(0)) != nullptr) {
        if (QWidget* w = child->widget()) {
            w->hide();
            w->deleteLater();
        }
        delete child;
    }

    const auto& filmes = repoFilmes.listarTodos();
    if (filmes.empty()) return;

    int larguraCard = 145;
    int larguraDisponivel = containerFilmesCards->width() - 20;
    if (larguraDisponivel < larguraCard) larguraDisponivel = larguraCard;

    int colunasMax = larguraDisponivel / larguraCard;
    if (colunasMax < 1) colunasMax = 1;
    if (colunasMax > static_cast<int>(filmes.size())) colunasMax = static_cast<int>(filmes.size());

    int col = 0, row = 0;
    for (size_t i = 0; i < filmes.size(); ++i) {
        const auto& filme = filmes[i];
        QPushButton* btnCard = new QPushButton(this);
        btnCard->setFixedSize(150, 200);
        btnCard->setCursor(Qt::PointingHandCursor);
        
        QString textoBtn = QString("🎬\n\n%1\n\n%2\n%3 min")
                                .arg(QString::fromStdString(filme.getTitulo()))
                                .arg(QString::fromStdString(filme.getGenero()))
                                .arg(filme.getDuracao());
        btnCard->setText(textoBtn);

        QString estiloCard = "QPushButton { background-color: #3d4d52; border: 2px solid #181c25; border-radius: 10px; font-size: 16px; font-weight: normal; color: #fbfef9; text-align: center; }"
                             "QPushButton:hover { border-color: #dec402; color: #dec402; }";
        if (static_cast<int>(i) == filmeSelecionadoIndex) {
            estiloCard = "QPushButton { background-color: #181c25; border: 3px solid #dec402; border-radius: 10px; font-size: 16px; color: #dec402; font-weight: bold; text-align: center; }";
        }
        btnCard->setStyleSheet(estiloCard);

        int idx = static_cast<int>(i);
        connect(btnCard, &QPushButton::clicked, this, [this, idx]() { onFilmeCardClicado(idx); });

        gridLayout->addWidget(btnCard, row, col, Qt::AlignCenter);
        col++;
        if (col >= colunasMax) { col = 0; row++; }
    }

    for (int c = 0; c < colunasMax; ++c) {
        gridLayout->setColumnStretch(c, 1);
    }
}

void Tela1SelecaoFilmeWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    reconstruirCardsFilmes();
}

void Tela1SelecaoFilmeWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    reconstruirCardsFilmes();
}

void Tela1SelecaoFilmeWidget::atualizarCatalogo() {
    reconstruirCardsFilmes();
    reconstruirListaSessoes(); // mantem a sessao ja escolhida, se houver
}

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
            QString itemTexto = QString("%1 - R$ %2 (Livres: %3)")
                                    .arg(QString::fromStdString(s.getDescricaoCompleta()))
                                    .arg(s.getPrecoBase(), 0, 'f', 2)
                                    .arg(s.getSala().contarAssentosLivres());
            listaSessoes->addItem(itemTexto);
        }
    }

    // Novas sessoes entram no fim da lista, entao a linha escolhida continua valida.
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
        msgBox.setText("Nao ha assentos livres suficientes nesta sessao.");
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