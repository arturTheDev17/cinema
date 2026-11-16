#ifndef FORMATACAO_H
#define FORMATACAO_H

#include <QLocale>
#include <QString>

/** Formata um valor em reais no padrao brasileiro, ex.: "R$ 1.234,50". */
inline QString formatarMoeda(double valor) {
    return QLocale(QLocale::Portuguese, QLocale::Brazil).toCurrencyString(valor, QStringLiteral("R$"));
}

#endif
