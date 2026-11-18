/**
 * @file BarraTituloWidget.h
 * @brief Declaração do widget BarraTituloWidget, barra de título personalizada multiplataforma.
 */

#ifndef BARRA_TITULO_WIDGET_H
#define BARRA_TITULO_WIDGET_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>

class QMouseEvent;

/**
 * @class BarraTituloWidget
 * @brief Barra de título própria utilizada pela janela principal sem moldura nativa.
 *
 * Mantém o tema escuro idêntico em qualquer plataforma sem depender de APIs específicas
 * do sistema operacional. Arrastar a janela utiliza QWindow::startSystemMove().
 * Emite apenas sinais; quem controla o estado da janela é a MainWindow.
 */
class BarraTituloWidget : public QFrame {
    Q_OBJECT

private:
    QLabel* lblIcone;          ///< Ícone da aplicação exibido à esquerda.
    QLabel* lblTitulo;         ///< Título exibido na barra.
    QPushButton* btnMinimizar; ///< Botão para minimizar a janela.
    QPushButton* btnMaximizar; ///< Botão para maximizar/restaurar a janela.
    QPushButton* btnFechar;    ///< Botão para fechar a aplicação.

protected:
    /**
     * @brief Inicia o movimento nativo da janela ao pressionar o botão esquerdo do mouse.
     * @param[in] event Evento de pressionamento do mouse.
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief Solicita alternância entre maximizado e normal no duplo clique esquerdo.
     * @param[in] event Evento de duplo clique do mouse.
     */
    void mouseDoubleClickEvent(QMouseEvent* event) override;

public:
    /**
     * @brief Constrói a barra de título personalizada.
     * @param[in] titulo Texto do título da janela.
     * @param[in] parent Widget pai no Qt (opcional).
     */
    explicit BarraTituloWidget(const QString& titulo, QWidget* parent = nullptr);

    /**
     * @brief Ajusta o símbolo e a dica do botão conforme a janela esteja maximizada ou não.
     * @param[in] maximizado true se maximizada; false se em tamanho normal.
     */
    void definirMaximizado(bool maximizado);

signals:
    /** @brief Emitido quando o usuário solicita minimizar a janela. */
    void minimizarSolicitado();

    /** @brief Emitido quando o usuário solicita maximizar ou restaurar a janela. */
    void alternarMaximizarSolicitado();

    /** @brief Emitido quando o usuário solicita fechar a aplicação. */
    void fecharSolicitado();
};

#endif