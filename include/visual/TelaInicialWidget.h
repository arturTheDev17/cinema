/**
 * @file TelaInicialWidget.h
 * @brief Declaração da tela de boas-vindas do totem TelaInicialWidget.
 */

#ifndef TELA_INICIAL_WIDGET_H
#define TELA_INICIAL_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

/**
 * @class TelaInicialWidget
 * @brief Tela de boas-vindas do totem com dois pontos de entrada para a jornada de compra.
 *
 * Apenas emite sinais; quem decide o que fazer com a escolha (limpar carrinho, trocar de tela) é a MainWindow:
 * - iniciarCompraCompleta(): filme -> sessão -> assentos -> bomboniere -> pagamento;
 * - iniciarSomenteBomboniere(): bomboniere -> pagamento (sem sessão de filme).
 */
class TelaInicialWidget : public QWidget {
    Q_OBJECT

private:
    QLabel* lblLogo;                   ///< Logotipo da aplicação.
    QLabel* lblTitulo;                 ///< Título principal do totem.
    QLabel* lblSubtitulo;              ///< Mensagem de orientação ao cliente.
    QPushButton* btnCompraCompleta;    ///< Card de início do fluxo completo com ingressos.
    QPushButton* btnSomenteBomboniere; ///< Card de início direto na bomboniere.

    /**
     * @brief Cria um card clicável padronizado para as opções da tela inicial.
     * @param[in] texto Texto exibido no card.
     * @return Ponteiro para o QPushButton criado.
     */
    QPushButton* criarCard(const QString& texto);

public:
    /**
     * @brief Constrói a tela inicial de boas-vindas.
     * @param[in] parent Widget pai no Qt (opcional).
     */
    explicit TelaInicialWidget(QWidget* parent = nullptr);

signals:
    /** @brief Emitido ao escolher a compra completa de ingressos e bomboniere. */
    void iniciarCompraCompleta();

    /** @brief Emitido ao escolher ir diretamente para a bomboniere sem sessão. */
    void iniciarSomenteBomboniere();
};

#endif