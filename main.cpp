#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <pages.h>
#include <QQmlContext>
#include <QObject>
#include <QDebug>
#include <QUrl>
#include "LevelEditorController.hpp"
#include "TilesetPalette.hpp"
#include "LevelCanvas.hpp"
#include "GameController.hpp"
#include "GameView.hpp"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    pages* myPage = new pages();

    // Register class for QML
    qmlRegisterType<LevelEditorController>("Cloudgate_game", 1, 0, "LevelEditorController");
    qmlRegisterType<TilesetPalette>("Cloudgate_game", 1, 0, "TilesetPalette");
    qmlRegisterType<LevelCanvas>("Cloudgate_game", 1, 0, "LevelCanvas");
    qmlRegisterType<GameController>("Cloudgate_game", 1, 0, "GameController");
    qmlRegisterType<GameView>("Cloudgate_game", 1, 0, "GameView");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("myMain", myPage); // the object will be available in QML with name "myGlobalObject"
    engine.load(QUrl(QStringLiteral("qrc:/Cloudgate_game/Main.qml")));

    QObject::connect(myPage, &pages::page_changed, [myPage]() {
        qInfo() << "Page chnaged to " << myPage->getPageInfo(myPage->page_value()) << " value " << myPage->page_value() ;
    });

    return app.exec();
}
