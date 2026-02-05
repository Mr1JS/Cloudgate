#include "include/GameView.hpp"
#include "game/include/MainWindow.hpp"
#include "game/include/Level.hpp"
#include <QQuickWindow>
#include <QKeyEvent>
#include <QDebug>
#include <QPainter>
#include <QDir>
#include <QCoreApplication>
#include <QFile>
#include <QMap>
#include <QRegion>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_surface.h>
#include <cstring>
#include <iostream>

GameView::GameView(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , m_running(false)
    , m_paused(false)
    , m_initialized(false)
    , m_gameWidth(800)
    , m_gameHeight(600)
{
    setAntialiasing(false);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setFocus(true);
    setActiveFocusOnTab(true);
    setClip(false);  // Clip deaktivieren, wir machen es manuell im paint()
    // WICHTIG: setze z-Wert niedrig, damit andere Items darüber liegen können
    setZ(0);

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &GameView::updateGame);

    // Initialize SDL
    initializeSDL();
}

GameView::~GameView()
{
    stopGame();
    cleanupSDL();
}

void GameView::initializeSDL()
{
    if (SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER) < 0)
    {
        qDebug() << "SDL could not initialize:" << SDL_GetError();
        return;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
    {
        qDebug() << "SDL_image could not initialize:" << IMG_GetError();
    }

    m_initialized = true;
}

void GameView::cleanupSDL()
{
    if (m_initialized)
    {
        IMG_Quit();
        SDL_Quit();
        m_initialized = false;
    }
}

void GameView::componentComplete()
{
    QQuickPaintedItem::componentComplete();
}

void GameView::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);

    // Verwende die Größe des Parent-Elements (ganzes Fenster)
    if (newGeometry.width() > 0 && newGeometry.height() > 0)
    {
        m_gameWidth = newGeometry.width();
        m_gameHeight = newGeometry.height();

        // Wenn das Spiel bereits läuft, müssen wir das MainWindow neu erstellen
        // mit der neuen Größe (für jetzt ignorieren wir Größenänderungen während des Spiels)
    }
}

void GameView::setLevelPath(const QString& path)
{
    if (m_levelPath != path)
    {
        m_levelPath = path;
        emit levelPathChanged();
    }
}

void GameView::setPaused(bool paused)
{
    if (m_paused != paused)
    {
        m_paused = paused;
        emit pausedChanged();
        update();  // Trigger repaint
    }
}

void GameView::startGame()
{
    if (m_running)
    {
        qDebug() << "Game is already running";
        return;
    }

    if (!m_initialized)
    {
        qDebug() << "SDL not initialized";
        return;
    }

    try {
        // Get absolute path to level.xml
        QString absoluteLevelPath = m_levelPath;
        if (absoluteLevelPath.isEmpty())
        {
            QDir appDir(QCoreApplication::applicationDirPath());
            appDir.cdUp();
            absoluteLevelPath = appDir.absoluteFilePath("res/level.xml");

            if (!QFile::exists(absoluteLevelPath))
            {
                QDir currentDir = QDir::current();
                absoluteLevelPath = currentDir.absoluteFilePath("res/level.xml");
            }
        }
        else if (!QDir::isAbsolutePath(absoluteLevelPath))
        {
            QDir appDir(QCoreApplication::applicationDirPath());
            appDir.cdUp();
            absoluteLevelPath = appDir.absoluteFilePath(absoluteLevelPath);

            if (!QFile::exists(absoluteLevelPath))
            {
                QDir currentDir = QDir::current();
                absoluteLevelPath = currentDir.absoluteFilePath(m_levelPath);
            }
        }

        qDebug() << "Starting game with level:" << absoluteLevelPath;
        qDebug() << "Level file exists:" << QFile::exists(absoluteLevelPath);

        if (!QFile::exists(absoluteLevelPath))
        {
            qDebug() << "Level file not found:" << absoluteLevelPath;
            return;
        }

        // Create game window with hidden window
        // Verwende aktuelle Größe vom Parent (ganzes Fenster)
        QRectF rect = boundingRect();
        int width = rect.width() > 0 ? rect.width() : 800;
        int height = rect.height() > 0 ? rect.height() : 1024;

        // Aktualisiere interne Größe
        m_gameWidth = width;
        m_gameHeight = height;

        qDebug() << "Creating game window with size:" << width << "x" << height;

        m_gameWindow = std::make_unique<jumper::MainWindow>(
            "Jumper Game",
            absoluteLevelPath.toStdString(),
            width,
            height
            );

        m_running = true;
        emit runningChanged();
        emit gameStarted();

        // Fokus setzen, damit Tastatur-Events empfangen werden
        setFocus(true);
        forceActiveFocus();

        // Start update timer (60 FPS)
        m_updateTimer->start(16);

        // Initial render
        update();

    }
    catch (const std::exception& e)
    {
        qDebug() << "Error starting game:" << e.what();
        m_running = false;
        emit runningChanged();
        emit gameStopped();
    }
}

void GameView::stopGame()
{
    if (!m_running)
    {
        return;
    }

    m_updateTimer->stop();
    m_running = false;
    m_gameWindow.reset();

    emit runningChanged();
    emit gameStopped();

    update();
}

void GameView::updateGame()
{
    if (!m_running || !m_gameWindow || m_paused)
    {
        // Wenn pausiert, nur repaint triggern (für statisches Bild)
        if (m_running && m_gameWindow) {
            update();
        }
        return;
    }

    // WICHTIG: SDL_PumpEvents() aufrufen, damit SDL die Event-Queue aktualisiert
    SDL_PumpEvents();

    // Process SDL events (but don't show window)
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        // Handle events if needed, but ignore SDL_QUIT
        if (e.type == SDL_QUIT) {
            // Ignorieren, da wir kein sichtbares Fenster haben
        }
    }

    // Erstelle ein Array mit Tastatur-Zuständen basierend auf unseren getrackten Zuständen
    // SDL_GetKeyboardState() gibt ein Array zurück, das wir nicht direkt modifizieren können
    // Stattdessen erstellen wir ein temporäres Array
    const int numKeys = 512; // SDL hat maximal 512 Scancodes
    Uint8* keystates = new Uint8[numKeys];
    memset(keystates, 0, numKeys);

    // Kopiere die echten SDL-Zustände
    const Uint8* sdlKeystates = SDL_GetKeyboardState(NULL);
    memcpy(keystates, sdlKeystates, numKeys);

    // Füge unsere getrackten Qt-Tastatur-Zustände hinzu
    for (auto it = m_keyStates.begin(); it != m_keyStates.end(); ++it)
    {
        SDL_Scancode scancode = convertQtKeyToSDLScancode(it.key());
        if (scancode != SDL_SCANCODE_UNKNOWN && scancode < numKeys)
        {
            keystates[scancode] = it.value() ? 1 : 0;
        }
    }

    // Update game with keyboard states
    m_gameWindow->update(keystates);

    // TODO: GameController should be implemented somewhere here

    // Prüfe, ob Actor außerhalb des Kamera-Bereichs ist (Game Over)
    // WICHTIG: Nur prüfen, wenn das Spiel läuft
    if(m_running && m_gameWindow && m_gameWindow->level())
    {
        // Use the new isGameOver() method that checks both HP and camera bounds
        if(m_gameWindow->level()->isGameOver())
        {
            // Game Over: Stoppe das Spiel
            stopGame();
            emit gameOver();
        }

        if(m_gameWindow && m_gameWindow->level() && m_gameWindow->level()->isLevelFinished())
        {
            stopGame();
            emit levelFinished();
        }
    }

    delete[] keystates;

    // Trigger repaint
    update();
}

void GameView::paint(QPainter *painter)
{
    if (!m_running || !m_gameWindow)
    {
        // Draw black background
        painter->fillRect(boundingRect(), Qt::black);
        painter->setPen(Qt::white);
        painter->drawText(boundingRect(), Qt::AlignCenter, "Klicke auf 'Spiel starten'");
        return;
    }

    // WICHTIG: Wenn pausiert, KEIN Rendering - das verhindert, dass GameView über das Menü rendert
    if (m_paused)
    {
        // Rendere nichts - das Menü liegt darüber
        return;
    }

    // Get SDL renderer
    SDL_Renderer* renderer = m_gameWindow->renderer();
    if (!renderer) {
        painter->fillRect(boundingRect(), Qt::black);
        return;
    }

    // Render game to SDL renderer
    m_gameWindow->render();

    // Copy SDL renderer content to QImage
    // Create a surface to read pixels from renderer
    int width = m_gameWidth > 0 ? m_gameWidth : 800;
    int height = m_gameHeight > 0 ? m_gameHeight : 600;
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32,
                                                0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!surface) {
        painter->fillRect(boundingRect(), Qt::black);
        return;
    }

    // Read pixels from renderer
    if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
                             surface->pixels, surface->pitch) != 0) {
        SDL_FreeSurface(surface);
        painter->fillRect(boundingRect(), Qt::black);
        return;
    }

    // Convert SDL surface to QImage
    QImage image(static_cast<uchar*>(surface->pixels),
                 surface->w, surface->h, surface->pitch,
                 QImage::Format_ARGB32);

    // Copy image (SDL surface owns the data, so we need a deep copy)
    QImage imageCopy = image.copy();

    // Speichere das Bild für Pause-Modus
    m_gameImage = imageCopy.copy();

    SDL_FreeSurface(surface);

    // Draw image to painter
    // Verwende SmoothPixmapTransform für bessere Qualität
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setRenderHint(QPainter::Antialiasing, false);

    painter->drawImage(boundingRect(), imageCopy);
}

void GameView::keyPressEvent(QKeyEvent *event)
{
    // ESC-Taste wird in QML behandelt (nicht an SDL weitergeben)
    if (event->key() == Qt::Key_Escape) {
        // ESC wird in SecondPage.qml behandelt
        QQuickPaintedItem::keyPressEvent(event);
        return;
    }

    if (!m_running || !m_gameWindow) {
        QQuickPaintedItem::keyPressEvent(event);
        return;
    }

    // Track key state
    m_keyStates[event->key()] = true;

    // Convert Qt key to SDL keycode and send SDL event
    SDL_Keycode keycode = convertQtKeyToSDL(event->key());
    if (keycode != SDLK_UNKNOWN) {
        SDL_Event sdlEvent;
        sdlEvent.type = SDL_KEYDOWN;
        sdlEvent.key.keysym.sym = keycode;
        sdlEvent.key.keysym.scancode = SDL_GetScancodeFromKey(keycode);
        sdlEvent.key.state = SDL_PRESSED;
        sdlEvent.key.repeat = 0;
        SDL_PushEvent(&sdlEvent);
    }

    QQuickPaintedItem::keyPressEvent(event);
}

void GameView::keyReleaseEvent(QKeyEvent *event)
{
    if (!m_running || !m_gameWindow) {
        QQuickPaintedItem::keyReleaseEvent(event);
        return;
    }

    // Track key state
    m_keyStates[event->key()] = false;

    // Convert Qt key to SDL keycode and send SDL event
    SDL_Keycode keycode = convertQtKeyToSDL(event->key());
    if (keycode != SDLK_UNKNOWN) {
        SDL_Event sdlEvent;
        sdlEvent.type = SDL_KEYUP;
        sdlEvent.key.keysym.sym = keycode;
        sdlEvent.key.keysym.scancode = SDL_GetScancodeFromKey(keycode);
        sdlEvent.key.state = SDL_RELEASED;
        sdlEvent.key.repeat = 0;
        SDL_PushEvent(&sdlEvent);
    }

    QQuickPaintedItem::keyReleaseEvent(event);
}

bool GameView::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        setFocus(true);
    }
    return QQuickPaintedItem::event(event);
}

SDL_Keycode GameView::convertQtKeyToSDL(int qtKey)
{
    // Map common Qt keys to SDL keycodes
    switch (qtKey) {
    case Qt::Key_Left: return SDLK_LEFT;
    case Qt::Key_Right: return SDLK_RIGHT;
    case Qt::Key_Up: return SDLK_UP;
    case Qt::Key_Down: return SDLK_DOWN;
    case Qt::Key_Space: return SDLK_SPACE;
    case Qt::Key_A: return SDLK_a;
    case Qt::Key_D: return SDLK_d;
    case Qt::Key_P: return SDLK_p;
    default: return SDLK_UNKNOWN;
    }
}

SDL_Scancode GameView::convertQtKeyToSDLScancode(int qtKey)
{
    // Map Qt keys directly to SDL scancodes (wichtig für SDL_GetKeyboardState)
    switch (qtKey) {
    case Qt::Key_Left: return SDL_SCANCODE_LEFT;
    case Qt::Key_Right: return SDL_SCANCODE_RIGHT;
    case Qt::Key_Up: return SDL_SCANCODE_UP;
    case Qt::Key_Down: return SDL_SCANCODE_DOWN;
    case Qt::Key_Space: return SDL_SCANCODE_SPACE;
    case Qt::Key_A: return SDL_SCANCODE_A;
    case Qt::Key_D: return SDL_SCANCODE_D;
    case Qt::Key_P: return SDL_SCANCODE_P;
    default: return SDL_SCANCODE_UNKNOWN;
    }
}
