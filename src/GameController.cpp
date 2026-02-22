/**
 * @file GameController.cpp
 * @brief Implementation of the GameController class for managing game lifecycle,
 *        threading, level loading and communication between Qt GUI and SDL game engine
 */

#include "include/GameController.hpp"
#include "game/include/MainWindow.hpp"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

GameController::GameController(QObject *parent)
    : QObject(parent)
    , m_running(false)
    , m_gameThread(nullptr)
{
    // Get absolute path to level.xml relative to executable
    QDir appDir(QCoreApplication::applicationDirPath());
    // Go up from build directory to project root
    appDir.cdUp();
    QString levelPath = appDir.absoluteFilePath("res/level.xml");

    // Fallback: try relative path from current working directory
    if (!QFile::exists(levelPath))
    {
        levelPath = "res/level.xml";
    }

    m_levelPath = levelPath;
}

GameController::~GameController()
{
    stopGame();
}

void GameController::setLevelPath(const QString& path)
{
    if (m_levelPath != path)
    {
        m_levelPath = path;
        emit levelPathChanged();
    }
}

void GameController::setRunning(bool running)
{
    if (m_running != running)
    {
        m_running = running;
        emit runningChanged();
    }
}

void GameController::startGame()
{
    if (m_running)
    {
        qDebug() << "Game is already running";
        return;
    }

    try
    {
        // Convert to absolute path if relative
        QString absoluteLevelPath = m_levelPath;
        if (!QDir::isAbsolutePath(absoluteLevelPath))
        {
            QDir appDir(QCoreApplication::applicationDirPath());
            appDir.cdUp();
            absoluteLevelPath = appDir.absoluteFilePath(m_levelPath);

            // If still doesn't exist, try from current working directory
            if (!QFile::exists(absoluteLevelPath))
            {
                QDir currentDir = QDir::current();
                absoluteLevelPath = currentDir.absoluteFilePath(m_levelPath);
            }
        }

        qDebug() << "Starting game with level:" << absoluteLevelPath;
        qDebug() << "Level file exists:" << QFile::exists(absoluteLevelPath);

        // Create game window in a separate thread
        m_gameThread = new QThread(this);

        // Create game window
        m_gameWindow = std::make_unique<jumper::MainWindow>(
            "Jumper Game",
            absoluteLevelPath.toStdString(),
            800,
            600
            );

        setRunning(true);
        emit gameStarted();

        // Run game loop
        if (m_gameWindow)
        {
            m_gameWindow->run();
        }

        setRunning(false);
        emit gameStopped();

    }
    catch (const std::exception& e)
    {
        qDebug() << "Error starting game:" << e.what();
        setRunning(false);
        emit gameStopped();
    }
}

void GameController::stopGame()
{
    if (!m_running)
    {
        return;
    }

    setRunning(false);
    m_gameWindow.reset();

    if (m_gameThread)
    {
        m_gameThread->quit();
        m_gameThread->wait();
        m_gameThread->deleteLater();
        m_gameThread = nullptr;
    }

    emit gameStopped();
}
