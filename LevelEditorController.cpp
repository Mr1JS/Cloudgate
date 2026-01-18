#include "LevelEditorController.hpp"
#include "TilesetPalette.hpp"
#include "LevelCanvas.hpp"
#include <QDebug>

LevelEditorController::LevelEditorController(QObject* parent)
    : QObject(parent)
{
    qDebug() << "LevelEditorController created";
}

void LevelEditorController::registerPalette(TilesetPalette* palette)
{
    if (m_palette != nullptr)
    {
        qWarning() << "Palette already registered, replacing...";
    }

    m_palette = palette;
    qDebug() << "TilesetPalette registered with controller";

    // Connect palette signals
    if (m_palette)
    {
        connect(m_palette, &TilesetPalette::tileSelected,
                this, &LevelEditorController::selectTile);
    }
}

void LevelEditorController::registerCanvas(LevelCanvas* canvas)
{
    if (m_canvas != nullptr)
    {
        qWarning() << "Canvas already registered, replacing...";
    }

    m_canvas = canvas;
    qDebug() << "LevelCanvas registered with controller";
}

void LevelEditorController::loadTileset(const QString& path, int tileWidth, int tileHeight, int offset)
{
    m_tilesetPath = path;
    m_tileWidth = tileWidth;
    m_tileHeight = tileHeight;
    m_tileOffset = offset;

    qDebug() << "=== CONTROLLER: Loading Tileset ===";
    qDebug() << "Path:" << path;
    qDebug() << "Tile size:" << tileWidth << "x" << tileHeight;
    qDebug() << "Offset:" << offset;

    // Load tileset into both palette and canvas
    if (m_palette)
    {
        m_palette->loadTileset(path, tileWidth, tileHeight, offset);
        qDebug() << "Tileset loaded into palette";
    }
    else
    {
        qWarning() << "Palette not registered!";
    }

    if (m_canvas)
    {
        m_canvas->setTileset(path, tileWidth, tileHeight, offset);
        qDebug() << "Tileset loaded into canvas";
    }
    else
    {
        qWarning() << "Canvas not registered!";
    }

    emit tilesetPathChanged();
    qDebug() << "===================================";
}

void LevelEditorController::setTileDimensions(int width, int height)
{
    m_tileWidth = width;
    m_tileHeight = height;

    qDebug() << "Tile dimensions changed to:" << width << "x" << height;

    // Reload tileset with new dimensions
    if (!m_tilesetPath.isEmpty())
    {
        loadTileset(m_tilesetPath, m_tileWidth, m_tileHeight, m_tileOffset);
    }
}

void LevelEditorController::newLevel(int gridWidth, int gridHeight)
{
    m_gridWidth = gridWidth;
    m_gridHeight = gridHeight;

    qDebug() << "Creating new level:" << gridWidth << "x" << gridHeight;

    if (m_canvas)
    {
        m_canvas->setGridWidth(gridWidth);
        m_canvas->setGridHeight(gridHeight);
        m_canvas->clearLevel();
        qDebug() << "New level created";
    }
    else
    {
        qWarning() << "Canvas not registered!";
    }
}

void LevelEditorController::clearLevel()
{
    qDebug() << "Clearing level...";

    if (m_canvas)
    {
        m_canvas->clearLevel();
        emit levelCleared();
        qDebug() << "Level cleared";
    }
    else
    {
        qWarning() << "Canvas not registered!";
    }
}

void LevelEditorController::saveLevel(const QString& path)
{
    qDebug() << "Saving level to:" << path;

    if (m_canvas)
    {
        m_canvas->saveLevel(path);
        emit levelSaved(path);
        qDebug() << "Level saved successfully";
    }
    else
    {
        qWarning() << "Canvas not registered!";
    }
}

void LevelEditorController::loadLevel(const QString& path)
{
    qDebug() << "Loading level from:" << path;

    if (m_canvas)
    {
        m_canvas->loadLevel(path);
        emit levelLoaded(path);
        qDebug() << "Level loaded successfully";
    }
    else
    {
        qWarning() << "Canvas not registered!";
    }
}

void LevelEditorController::selectTile(int tileIndex)
{
    m_selectedTileIndex = tileIndex;

    qDebug() << "Controller: Tile" << tileIndex << "selected";

    // Update canvas with selected tile
    if (m_canvas)
    {
        m_canvas->placeTile(tileIndex);
    }
    else
    {
        qWarning() << "Canvas not registered!";
    }

    emit selectedTileChanged();
}
