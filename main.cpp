#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <pages.h>
#include <QQmlContext>
#include <QObject>
#include <QDebug>
#include <LevelEditor.hpp>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    pages* myPage = new pages();

    LevelEditor editor;
    editor.loadTileset(":/resources/images/tileset.png");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.rootContext()->setContextProperty("myMain", myPage); // the object will be available in QML with name "myGlobalObject"
    engine.rootContext()->setContextProperty("levelEditor", &editor);
    qmlRegisterType<LevelEditor>("Cloudgate_game", 1, 0, "LevelEditor");
    engine.loadFromModule("Cloudgate_game", "Main");

    QObject::connect(myPage, &pages::page_changed, [myPage]() {
        qInfo() << "Page chnaged to " << myPage->getPageInfo(myPage->page_value()) << " value " << myPage->page_value() ;
    });

    return app.exec();
}
