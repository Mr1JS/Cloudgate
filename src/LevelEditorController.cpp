#include "include/LevelEditorController.hpp"
#include "include/TilesetPalette.hpp"
#include "include/LevelCanvas.hpp"
#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>

LevelEditorController::LevelEditorController(QObject *parent)
    : QObject(parent)
{
    qDebug() << "LevelEditorController created";

    // Initialize last save path to user's documents folder
    m_lastSavePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
}

void LevelEditorController::registerPalette(TilesetPalette *palette)
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

void LevelEditorController::registerCanvas(LevelCanvas *canvas)
{
    if (m_canvas != nullptr)
    {
        qWarning() << "Canvas already registered, replacing...";
    }

    m_canvas = canvas;
    qDebug() << "LevelCanvas registered with controller";
}

void LevelEditorController::loadTileset(const QString &path, int tileWidth, int tileHeight, int offset, int endIndex)
{
    m_tilesetPath = path;
    m_tileWidth = tileWidth;
    m_tileHeight = tileHeight;
    m_tileOffset = offset;
    m_endIndex = endIndex;

    qDebug() << "=== CONTROLLER: Loading Tileset ===";
    qDebug() << "Path:" << path;
    qDebug() << "Tile size:" << tileWidth << "x" << tileHeight;
    qDebug() << "Offset:" << offset;

    // Load tileset into both palette and canvas
    if (m_palette)
    {
        m_palette->loadTileset(path, tileWidth, tileHeight, offset, endIndex);
        qDebug() << "Tileset loaded into palette";
    }
    else
    {
        qWarning() << "Palette not registered!";
    }

    if (m_canvas)
    {
        m_canvas->setTileset(path, tileWidth, tileHeight, offset, endIndex);
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
        loadTileset(m_tilesetPath, m_tileWidth, m_tileHeight, m_tileOffset, m_endIndex);
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

void LevelEditorController::saveLevel()
{
    qDebug() << "Opening save dialog...";

    if (!m_canvas)
    {
        qWarning() << "Canvas not registered!";
        return;
    }

    // Open file dialog for saving
    QString fileName = QFileDialog::getSaveFileName(
        nullptr,                                 // parent widget
        tr("Save Level"),                        // dialog title
        m_lastSavePath + "/level.dat",           // default path
        tr("Level Files (*.dat);;All Files (*)") // file filter
    );

    // If user cancelled the dialog
    if (fileName.isEmpty())
    {
        qDebug() << "Save cancelled by user";
        return;
    }

    // Ensure .dat extension
    if (!fileName.endsWith(".dat", Qt::CaseInsensitive))
    {
        fileName += ".dat";
    }

    // Remember directory for next time
    QFileInfo fileInfo(fileName);
    m_lastSavePath = fileInfo.absolutePath();

    // Save the level
    qDebug() << "Saving level to:" << fileName;
    m_canvas->saveLevel(fileName);
    emit levelSaved(fileName);
    qDebug() << "Level saved successfully";
}

void LevelEditorController::loadLevel()
{
    qDebug() << "Opening load dialog...";

    if (!m_canvas)
    {
        qWarning() << "Canvas not registered!";
        return;
    }

    // Open file dialog for loading
    QString fileName = QFileDialog::getOpenFileName(
        nullptr,                                 // parent widget
        tr("Load Level"),                        // dialog title
        m_lastSavePath,                          // starting directory
        tr("Level Files (*.dat);;All Files (*)") // file filter
    );

    // If user cancelled the dialog
    if (fileName.isEmpty())
    {
        qDebug() << "Load cancelled by user";
        return;
    }

    // Remember directory for next time
    QFileInfo fileInfo(fileName);
    m_lastSavePath = fileInfo.absolutePath();

    // Load the level
    qDebug() << "Loading level from:" << fileName;
    m_canvas->loadLevel(fileName);
    emit levelLoaded(fileName);
    qDebug() << "Level loaded successfully";
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

// switched to extra Tiles / 1x2 Tiles
void LevelEditorController::toggleExtraTileset(bool enabled)
{
    if (!m_palette || !m_canvas)
    {
        return;
    }
    m_palette->setExtraTiles(enabled);
    m_canvas->setExtraTiles(enabled);
}
