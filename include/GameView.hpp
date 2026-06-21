/**
 * @file GameView.hpp
 * @brief Defines the GameView class for rendering the game in a Qt Quick item
 */

#ifndef GAMEVIEW_HPP
#define GAMEVIEW_HPP

#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QImage>
#include <QKeyEvent>
#include <QMap>
#include <SDL.h>
#include <memory>
#include <QString>
#include <cstring>
#include <mutex>

// Forward declarations
namespace jumper
{
class MainWindow;
class Level;
}

class GameView : public QQuickPaintedItem
{
    Q_OBJECT

    /// Path to the level xml file (bridge to qml)
    Q_PROPERTY(QString levelPath READ levelPath WRITE setLevelPath NOTIFY levelPathChanged)

    /// State of game execution (bridge to qml)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)

    /// Pause state of the game (bridge to qml)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)

public:

    /// QObject based constructor
    explicit GameView(QQuickItem *parent = nullptr);

    /// Destructor
    ~GameView();

    /// @return The current level path
    QString levelPath() const { return m_levelPath; }

    /// Set the path for the level xml
    void setLevelPath(const QString& path);

    /// @return Current game running state
    bool running() const { return m_running; }

    /// @return Current game pause state
    bool paused() const { return m_paused; }

    /// Set the pause state
    void setPaused(bool paused);

    /// Rendering method for the tiles
    void paint(QPainter *painter) override;

public slots:

    /// Start the game loop and timer
    void startGame();

    /// Stop the game
    void stopGame();

    /// Update game logic and trigger repaint
    void updateGame();

signals:

    /// Emit new level path
    void levelPathChanged();

    /// Emit new running state
    void runningChanged();

    /// Emit new pause state
    void pausedChanged();

    /// Emit game started
    void gameStarted();

    /// Emit game stopped
    void gameStopped();

    /// Emit game over
    void gameOver();

    /// Emit level finished
    void levelFinished();

protected:

    /// Called when the component is fully loaded
    void componentComplete() override;

    /// resizing of visuals
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    /// Handle keyboard presses
    void keyPressEvent(QKeyEvent *event) override;

    /// Handle keyboard releases
    void keyReleaseEvent(QKeyEvent *event) override;

    /// event handler
    bool event(QEvent *event) override;

private:

    /// Initialize SDL systems and window
    void initializeSDL();

    /// Clean up SDL systems
    void cleanupSDL();

    /// Convert SDL surface to QImage for rendering
    void renderToImage();

    /// Convert Qt key to SDL keycode
    SDL_Keycode convertQtKeyToSDL(int qtKey);

    /// Convert Qt key to SDL scancode
    SDL_Scancode convertQtKeyToSDLScancode(int qtKey);

    QMap<int, bool>                     m_keyStates; /// Track keyboard state manually
    QString                             m_levelPath; /// Path to level xml
    std::unique_ptr<jumper::MainWindow> m_gameWindow; /// Pointer to SDL game window
    QTimer*                             m_updateTimer; /// Timer for the game loop
    bool                                m_running; /// State of game running
    bool                                m_paused; /// State of game paused
    bool                                m_initialized; /// Initialization flag
    QImage                              m_gameImage; /// Buffer for currently rendered frame
    int                                 m_gameWidth; /// Width of game area
    int                                 m_gameHeight; /// Height of game area
    mutable std::recursive_mutex        m_gameMutex; /// Mutes for thread-safe rendering
};

#endif // GAMEVIEW_HPP
