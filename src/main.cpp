/**
 * @file main.cpp
 * @brief Main entry point for the Qt application
 */

#include <QApplication>
#include <QQmlApplicationEngine>
#include <include/pages.h>
#include <QQmlContext>
#include <QObject>
#include <QDebug>
#include <QQuickWindow>
#include <QtGlobal>
#include "include/LevelEditorController.hpp"
#include "include/TilesetPalette.hpp"
#include "include/LevelCanvas.hpp"
#include "include/GameController.hpp"
#include "include/GameView.hpp"
#include "include/LevelSelector.hpp"
#include "include/ActorSelector.hpp"


int main(int argc, char *argv[])
{
    // style for buttons in the qml --> Leveleditor.qml
    qputenv("QT_QUICK_CONTROLS_STYLE", QByteArray("Fusion"));

#ifdef Q_OS_LINUX
    // Lab machines often run compositor/driver combinations where threaded
    // render loops with embedded SDL readback are unstable.
    if (qEnvironmentVariableIsEmpty("QSG_RENDER_LOOP"))
    {
        qputenv("QSG_RENDER_LOOP", QByteArray("basic"));
    }
    if (qEnvironmentVariableIsEmpty("JUMPER_SDL_SOFTWARE"))
    {
        qputenv("JUMPER_SDL_SOFTWARE", QByteArray("1"));
    }
#endif

    // need for rednering with SDL2 and not OpenGL
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Software);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    pages* myPage = new pages();

    // Register class for QML
    qmlRegisterType<LevelEditorController>("Cloudgate_game", 1, 0, "LevelEditorController");
    qmlRegisterType<TilesetPalette>("Cloudgate_game", 1, 0, "TilesetPalette");
    qmlRegisterType<LevelCanvas>("Cloudgate_game", 1, 0, "LevelCanvas");
    qmlRegisterType<GameController>("Cloudgate_game", 1, 0, "GameController");
    qmlRegisterType<GameView>("Cloudgate_game", 1, 0, "GameView");
    qmlRegisterType<LevelSelector>("Cloudgate_game", 1, 0, "LevelSelector");
    qmlRegisterType<ActorSelector>("Cloudgate_game", 1, 0, "ActorSelector");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("myMain", myPage); // the object will be available in QML with name "myGlobalObject"

    // CHECK QT VERSION
    const bool isQt6_5_or_newer = (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0));
    const QUrl mainQmlUrl = isQt6_5_or_newer
                                ? QString("qrc:/qt/qml/Cloudgate_game/qml/Main.qml")
                                : QString("qrc:/Cloudgate_game/qml/Main.qml");

    qDebug() << "Qt Version:" << QT_VERSION_STR;
    qDebug() << "Loading QML from:" << mainQmlUrl;
    engine.load(mainQmlUrl);
    //

    QObject::connect(myPage, &pages::page_changed, [myPage]() {
        qInfo() << "Page chnaged to " << myPage->getPageInfo(myPage->page_value()) << " value " << myPage->page_value() ;
    });

    return app.exec();
}
