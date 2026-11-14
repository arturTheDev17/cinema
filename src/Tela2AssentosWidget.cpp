#include "Tela2AssentosWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFrame>

Tela2AssentosWidget::Tela2AssentosWidget(Carrinho& carrinho, QWidget* parent)
    : QWidget(parent), carrinho(carrinho) {

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    QVBoxLayout* areaSalaLayout = new QVBoxLayout();

    QLabel* lblTelaCinema = new QLabel("TELA DO CINEMA", this);
    lblTelaCinema->setAlignment(Qt::AlignCenter);
    lblTelaCinema->setStyleSheet("background-color: #fbfef9; color: #181c25; font-weight: bold; font-size: 14px; padding: 10px; border-radius: 6px; margin: 5px");
    areaSalaLayout->addWidget(lblTelaCinema);

    QWidget* containerGrade = new QWidget(this);
    gridAssentos = new QGridLayout(containerGrade);
    gridAssentos->setSpacing(8);
    areaSalaLayout->addWidget(containerGrade, 1);

    lblInstrucao = new QLabel("Selecione os assentos", this);
    lblInstrucao->setStyleSheet("font-size: 14px; font-weight: normal; color: #dec402; margin-bottom: 6px;");
    areaSalaLayout->addWidget(lblInstrucao);

    QHBoxLayout* botoesNavLayout = new QHBoxLayout();

    btnVoltar = new QPushButton("Voltar", this);
    btnVoltar->setMinimumHeight(45);
    btnVoltar->setCursor(Qt::PointingHandCursor);

    btnAvancar = new QPushButton("Prosseguir para Bomboniere", this);
    btnAvancar->setObjectName("btnAvancarAssentos");
    btnAvancar->setMinimumHeight(45);
    btnAvancar->setCursor(Qt::PointingHandCursor);
    btnAvancar->setEnabled(false);

    botoesNavLayout->addWidget(btnVoltar);
    botoesNavLayout->addWidget(btnAvancar);
    areaSalaLayout->addLayout(botoesNavLayout);

    sidebar = new SidebarCarrinhoWidget(this);
    sidebar->definirModoExpandido(false);

    mainLayout->addLayout(areaSalaLayout, 2);
    mainLayout->addWidget(sidebar, 1);

    connect(btnVoltar, &QPushButton::clicked, this, &Tela2AssentosWidget::onVoltarClicado);
    connect(btnAvancar, &QPushButton::clicked, this, &Tela2AssentosWidget::onAvancarClicado);
}

void Tela2AssentosWidget::carregarSessaoAtual() {
    for (Assento* a : assentosSelecionados) {
        a->liberar();
    }
    assentosSelecionados.clear();
    reconstruirGrade();
    atualizarEstadoBotoes();
    sidebar->atualizar(carrinho, false);
}

void Tela2AssentosWidget::reconstruirGrade() {
    QLayoutItem* child;
    while ((child = gridAssentos->takeAt(0)) != nullptr) {
        if (child->widget()) delete child->widget();
        delete child;
    }
    botoesAssentos.clear();

    Sessao* sessao = carrinho.getSessao();
    if (!sessao) return;

    Sala& sala = sessao->getSala();
    int fileiras = sala.getQuantidadeFileiras();
    int colunas = sala.getQuantidadeColunas();

    botoesAssentos.resize(fileiras, std::vector<QPushButton*>(colunas, nullptr));

    for (int f = 1; f <= fileiras; ++f) {
        for (int c = 1; c <= colunas; ++c) {
            Assento& assento = sala.getAssento(f, c);
            QPushButton* btn = new QPushButton(QString::fromStdString(assento.getIdentificador()), this);
            btn->setMinimumSize(48, 40);
            btn->setCursor(Qt::PointingHandCursor);

            if (assento.getStatus() == SituacaoAssento::OCUPADO) {
                btn->setStyleSheet("background-color: #2b353b; color: #777777; border: 1px solid #3d4d52; border-radius: 4px; font-weight: bold;");
                btn->setEnabled(false);
                btn->setCursor(Qt::ForbiddenCursor);
            } else {
                btn->setStyleSheet("background-color: #7FBB3A; color: #181c25; border-radius: 4px; font-weight: bold;");
            }

            connect(btn, &QPushButton::clicked, this, [this, f, c]() {
                onAssentoClicado(f, c);
            });

            gridAssentos->addWidget(btn, f - 1, c - 1);
            botoesAssentos[f - 1][c - 1] = btn;
        }
    }
}

void Tela2AssentosWidget::onAssentoClicado(int fileira, int coluna) {
    Sessao* sessao = carrinho.getSessao();
    if (!sessao) return;

    Assento& assento = sessao->getSala().getAssento(fileira, coluna);
    QPushButton* btn = botoesAssentos[fileira - 1][coluna - 1];
    int limiteIngressos = carrinho.getQuantidadeIngressos();

    if (assento.getStatus() == SituacaoAssento::LIVRE) {
        if (static_cast<int>(assentosSelecionados.size()) < limiteIngressos) {
            assento.reservar();
            assentosSelecionados.push_back(&assento);
            btn->setStyleSheet("background-color: #D32222; color: #fbfef9; border-radius: 4px; font-weight: bold;");
        }
    } else if (assento.getStatus() == SituacaoAssento::RESERVADO) {
        assento.liberar();
        auto it = std::find(assentosSelecionados.begin(), assentosSelecionados.end(), &assento);
        if (it != assentosSelecionados.end()) {
            assentosSelecionados.erase(it);
        }
        btn->setStyleSheet("background-color: #7FBB3A; color: #181c25; border-radius: 4px; font-weight: bold;");
    }

    atualizarEstadoBotoes();
    if (static_cast<int>(assentosSelecionados.size()) == limiteIngressos) {
        carrinho.vincularAssentosAosIngressos(assentosSelecionados);
    } else {
        std::vector<Assento*> vazios(limiteIngressos, nullptr);
        carrinho.vincularAssentosAosIngressos(vazios);
    }
    sidebar->atualizar(carrinho, false);
}

void Tela2AssentosWidget::atualizarEstadoBotoes() {
    int necessarios = carrinho.getQuantidadeIngressos();
    int escolhidos = static_cast<int>(assentosSelecionados.size());
    lblInstrucao->setText(QString("Selecione exatamente %1 assento(s) (%2/%1 escolhidos)")
                            .arg(necessarios)
                            .arg(escolhidos));
    btnAvancar->setEnabled(escolhidos == necessarios && necessarios > 0);
}

void Tela2AssentosWidget::onVoltarClicado() {
    for (Assento* a : assentosSelecionados) {
        a->liberar();
    }
    assentosSelecionados.clear();
    emit voltarParaFilmes();
}

void Tela2AssentosWidget::onAvancarClicado() {
    int necessarios = carrinho.getQuantidadeIngressos();
    int escolhidos = static_cast<int>(assentosSelecionados.size());

    if (escolhidos != necessarios) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Atenção");
        msgBox.setText(QString("A quantidade de assentos selecionados (%1) nao bate com a quantidade de ingressos (%2).").arg(escolhidos).arg(necessarios));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    if (carrinho.vincularAssentosAosIngressos(assentosSelecionados)) {
        emit avancarParaBomboniere();
    }
}