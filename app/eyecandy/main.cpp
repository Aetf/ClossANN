#include <QApplication>
#include <QDesktopWidget>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QStyle>
#include <QtQml>
#include "logic/uihandler.h"

#ifdef QCUSTOMPLOT_USE_LIBRARY
#include <qcustomplot.h>
#include "widgets/mainwindow.h"
int runQtWidget(int argc, char *argv[]);
#endif

void registerRecordsModel();
int runQtQuick(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    if (argc > 1) {
        switch (argv[1][0]) {
#ifdef QCUSTOMPLOT_USE_LIBRARY
        case 'w':
            return runQtWidget(argc, argv);
#endif
        case 'q':
        default:
            return runQtQuick(argc, argv);
        }
    }
    return runQtQuick(argc, argv);
}

#ifdef QCUSTOMPLOT_USE_LIBRARY
int runQtWidget(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow w;
    w.show();
    return app.exec();
}
#endif

int runQtQuick(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    registerRecordsModel();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    // get ApplicationWindow object and center it
    Q_ASSERT(!engine.rootObjects().isEmpty());
    auto *const window = qobject_cast<QQuickWindow *>(engine.rootObjects().front());
    Q_ASSERT(window);
//    window->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, window->size(),
//                                            qApp->desktop()->availableGeometry()));
    return app.exec();
}

void registerRecordsModel()
{
    qmlRegisterSingletonType<UIHandler>("UCWNeuralNet", 1, 0, "Handler",
                                        UIHandler::UIHandlerProvider);
}
