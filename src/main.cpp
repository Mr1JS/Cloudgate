<<<<<<< HEAD
#include <QApplication>
=======
#include <QGuiApplication>
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557
#include <QQmlApplicationEngine>
#include <include/pages.h>
#include <QQmlContext>
#include <QObject>
#include <QDebug>
<<<<<<< HEAD
#include <QQuickWindow>
#include <QtGlobal>
#include "include/LevelEditorController.hpp"
#include "include/TilesetPalette.hpp"
#include "include/LevelCanvas.hpp"
#include "include/GameController.hpp"
#include "include/GameView.hpp"
=======
#include "include/LevelEditorController.hpp"
#include "include/TilesetPalette.hpp"
#include "include/LevelCanvas.hpp"
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557


int main(int argc, char *argv[])
{
<<<<<<< HEAD
    // style for buttons in the qml --> Leveleditor.qml
    qputenv("QT_QUICK_CONTROLS_STYLE", QByteArray("Fusion"));

    // need for rednering with SDL2 and not OpenGL
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);

    QApplication app(argc, argv);
=======
    QGuiApplication app(argc, argv);
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557

    QQmlApplicationEngine engine;

    pages* myPage = new pages();

    // Register class for QML
    qmlRegisterType<LevelEditorController>("Cloudgate_game", 1, 0, "LevelEditorController");
    qmlRegisterType<TilesetPalette>("Cloudgate_game", 1, 0, "TilesetPalette");
    qmlRegisterType<LevelCanvas>("Cloudgate_game", 1, 0, "LevelCanvas");
<<<<<<< HEAD
    qmlRegisterType<GameController>("Cloudgate_game", 1, 0, "GameController");
    qmlRegisterType<GameView>("Cloudgate_game", 1, 0, "GameView");
=======
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("myMain", myPage); // the object will be available in QML with name "myGlobalObject"
<<<<<<< HEAD

    // CHECK QT VERSION
    const bool isQt6_5_or_newer = (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0));
    const QUrl mainQmlUrl = isQt6_5_or_newer
                                ? QString("qrc:/qt/qml/Cloudgate_game/qml/Main.qml")
                                : QString("qrc:/Cloudgate_game/qml/Main.qml");

    qDebug() << "Qt Version:" << QT_VERSION_STR;
    qDebug() << "Loading QML from:" << mainQmlUrl;
    engine.load(mainQmlUrl);
    //
=======
    engine.loadFromModule("Cloudgate_game", "Main");
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557

    QObject::connect(myPage, &pages::page_changed, [myPage]() {
        qInfo() << "Page chnaged to " << myPage->getPageInfo(myPage->page_value()) << " value " << myPage->page_value() ;
    });

    return app.exec();
}
