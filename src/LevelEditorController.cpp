#include "include/LevelEditorController.hpp"
#include "include/TilesetPalette.hpp"
#include "include/LevelCanvas.hpp"
#include <QDebug>
#include <QFileDialog>

LevelEditorController::LevelEditorController(QObject *parent)
    : QObject(parent)
{
    qDebug() << "LevelEditorController created";
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
        // Load the Tile-names from the XML file
        m_palette->loadTileNames("res/RulesTiles.xml");
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

    // Open QFileDialog to get save path
    QString file_name = QFileDialog::getSaveFileName(
        nullptr,
        "Save Level",
        "level_master.xml",
        "Level Files (*.xml)");

    // If user cancelled
    if (file_name.isEmpty())
    {
        qDebug() << "Save cancelled by user";
        return;
    }

    // Ensure .xml extension
    if (!file_name.endsWith(".xml", Qt::CaseInsensitive))
    {
        file_name += ".xml";
    }

    // Open file for writing
    QFile file(file_name);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qWarning() << "Could not open file for writing:" << file_name;
        return;
    }

    // Save level
    m_canvas->saveLevel(file_name);
    file.close();

    emit levelSaved(file_name);
    qDebug() << "Level saved to:" << file_name;
}

void LevelEditorController::loadLevel()
{
    qDebug() << "Opening load dialog...";

    if (!m_canvas)
    {
        qWarning() << "Canvas not registered!";
        return;
    }

    // Open QFileDialog to get load path
    QString file_name = QFileDialog::getOpenFileName(
        nullptr,
        "Open Level",
        "",
        "Level Files (*.xml)");

    // If user cancelled
    if (file_name.isEmpty())
    {
        qDebug() << "Load cancelled by user";
        return;
    }

    // Open file for reading
    QFile file(file_name);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qWarning() << "Could not open file for reading:" << file_name;
        return;
    }

    // Load level
    m_canvas->loadLevel(file_name);
    file.close();

    emit levelLoaded(file_name);
    qDebug() << "Level loaded from:" << file_name;
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