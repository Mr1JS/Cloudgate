#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include <QObject>
#include <QThread>
#include <QString>
#include <QFile>
#include <memory>

namespace jumper {
    class MainWindow;
}

class GameController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString levelPath READ levelPath WRITE setLevelPath NOTIFY levelPathChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)

public:
    explicit GameController(QObject *parent = nullptr);
    ~GameController();

    QString levelPath() const { return m_levelPath; }
    void setLevelPath(const QString& path);
    bool running() const { return m_running; }

public slots:
    void startGame();
    void stopGame();

signals:
    void levelPathChanged();
    void runningChanged();
    void gameStarted();
    void gameStopped();

private:
    void setRunning(bool running);

    QString m_levelPath;
    std::unique_ptr<jumper::MainWindow> m_gameWindow;
    QThread* m_gameThread;
    bool m_running;
};

#endif // GAMECONTROLLER_HPP
