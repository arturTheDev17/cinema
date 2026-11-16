#include <QApplication>
#include <QtGlobal>
#include "MainWindow.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
#include <QStyleHints>
#endif

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("CinemaMecha");

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    // Pede o tema escuro ao proprio Qt: barra de titulo escura sem codigo especifico de SO.
    QApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
#endif

    MainWindow w;
    w.show();
    return app.exec();
}
