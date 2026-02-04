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

// Forward declarations
namespace jumper {
class MainWindow;
class Level;
}

class GameView : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString levelPath READ levelPath WRITE setLevelPath NOTIFY levelPathChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)

public:
    explicit GameView(QQuickItem *parent = nullptr);
    ~GameView();

    QString levelPath() const { return m_levelPath; }
    void setLevelPath(const QString& path);
    bool running() const { return m_running; }
    bool paused() const { return m_paused; }
    void setPaused(bool paused);

    void paint(QPainter *painter) override;

    // Setze clipEnabled, damit wir UI-Bereiche ausschließen können
    // QQuickPaintedItem::clip ist standardmäßig false

public slots:
    void startGame();
    void stopGame();
    void updateGame();

signals:
    void levelPathChanged();
    void runningChanged();
    void pausedChanged();
    void gameStarted();
    void gameStopped();
    void gameOver();
    void levelFinished();

protected:
    void componentComplete() override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    bool event(QEvent *event) override;

private:
    void initializeSDL();
    void cleanupSDL();
    void renderToImage();
    SDL_Keycode convertQtKeyToSDL(int qtKey);
    SDL_Scancode convertQtKeyToSDLScancode(int qtKey);

    // Track keyboard state manually
    QMap<int, bool> m_keyStates;

    QString m_levelPath;
    std::unique_ptr<jumper::MainWindow> m_gameWindow;
    QTimer* m_updateTimer;
    bool m_running;
    bool m_paused;
    bool m_initialized;
    QImage m_gameImage;
    int m_gameWidth;
    int m_gameHeight;
};

#endif // GAMEVIEW_HPP
