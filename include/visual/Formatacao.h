/**
 * @file Formatacao.h
 * @brief Funções auxiliares inline para formatação de exibição na interface gráfica.
 */

#ifndef FORMATACAO_H
#define FORMATACAO_H

#include <QLocale>
#include <QString>

/**
 * @brief Formata um valor em reais no padrão monetário brasileiro (ex.: "R$ 1.234,50").
 * @param[in] valor Valor numérico em reais.
 * @return QString formatada de acordo com a localidade pt_BR.
 */
inline QString formatarMoeda(double valor) {
    return QLocale(QLocale::Portuguese, QLocale::Brazil).toCurrencyString(valor, QStringLiteral("R$"));
}

#endif