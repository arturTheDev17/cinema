#ifndef TELA_INICIAL_WIDGET_H
#define TELA_INICIAL_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

/**
 * Tela de boas-vindas do totem.
 *
 * Oferece dois pontos de entrada e apenas emite sinais; quem decide o que fazer
 * com a escolha (limpar carrinho, trocar de tela) e a MainWindow.
 *   - iniciarCompraCompleta():     filme -> sessao -> assentos -> bomboniere -> pagamento
 *   - iniciarSomenteBomboniere():  bomboniere -> pagamento (sem sessao de filme)
 */
class TelaInicialWidget : public QWidget {
    Q_OBJECT

private:
    QLabel* lblLogo;
    QLabel* lblTitulo;
    QLabel* lblSubtitulo;
    QPushButton* btnCompraCompleta;
    QPushButton* btnSomenteBomboniere;

    QPushButton* criarCard(const QString& texto);

public:
    explicit TelaInicialWidget(QWidget* parent = nullptr);

signals:
    void iniciarCompraCompleta();
    void iniciarSomenteBomboniere();
};

#endif
