/**
 * @file GameController.hpp
 * @brief Defines the GameController class for managing game lifecycle and threading
 */

#ifndef GAMECONTROLLER_HPP
#define GAMECONTROLLER_HPP

#include <QObject>
#include <QThread>
#include <QString>
#include <QFile>
#include <memory>

namespace jumper
{
class MainWindow;
}

class GameController : public QObject
{
    Q_OBJECT

    /// Absolute path to xml level file (bridge to qml)
    Q_PROPERTY(QString levelPath READ levelPath WRITE setLevelPath NOTIFY levelPathChanged)

    /// State of game / is game running (bridge to qml)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)

public:
    /// QObject based Constructor
    explicit GameController(QObject *parent = nullptr);

    /// Destructor
    ~GameController();

    /// @return xml levelPath
    QString levelPath() const { return m_levelPath; }

    /// Set xml levelPath
    void setLevelPath(const QString& path);

    /// @return game state / if game is running
    bool running() const { return m_running; }

public slots:

    /// Start the game
    void startGame();

    /// Stop the game
    void stopGame();

signals:

    /// Emit new level Path
    void levelPathChanged();

    /// Emit new game state
    void runningChanged();

    /// Emit game started
    void gameStarted();

    /// Emit game stopped
    void gameStopped();

private:

    /// Switch state of game
    void setRunning(bool running);

    QString                             m_levelPath; /// Path to xml level
    std::unique_ptr<jumper::MainWindow> m_gameWindow; /// Pointer to SDL game window
    QThread*                            m_gameThread; /// Thread for game logic
    bool                                m_running; /// State of game
};

#endif // GAMECONTROLLER_HPP
