#include "include/LevelEditorController.hpp"
#include "include/TilesetPalette.hpp"
#include "include/LevelCanvas.hpp"
#include <QDebug>
#include <QFileDialog>
#include "game/io/BaseHdf5IO.hpp"
#include "game/io/TileSetIO.hpp"
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include "game/include/Util.hpp"

using LevelHdf5IO = jumper::BaseHdf5IO<jumper::hdf5features::TileSetIO>;

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
    // connect signals for selection
    connect(m_palette, &TilesetPalette::tileSelected,
            this, &LevelEditorController::selectTile);

    // connect signals for  tileset loading
    connect(this, &LevelEditorController::tilesetReady,
            m_palette, &TilesetPalette::setTileset);

    qDebug() << "TilesetPalette registered with controller";
}

void LevelEditorController::registerCanvas(LevelCanvas *canvas)
{
    if (m_canvas != nullptr)
    {
        qWarning() << "Canvas already registered, replacing...";
    }

    m_canvas = canvas;

    // connect signal tileset loading
    connect(this, &LevelEditorController::tilesetReady,
            m_canvas, &LevelCanvas::setTileset);

    qDebug() << "LevelCanvas registered with controller";
}

void LevelEditorController::loadTileset(const QString &path, int tileWidth, int tileHeight, int offset, int endIndex)
{

    // Load tileset into both palette and canvas
    if (m_palette)
    {
        m_palette->loadTileset(path, tileWidth, tileHeight, offset, endIndex);
        // Load the Tile-names from the XML file
        m_palette->loadTileNames("res/RulesTiles.xml");
        qDebug() << "Tileset loaded into palette";
    }
    catch (const std::exception &e)
    {
        qWarning() << "Failed to read XML:" << e.what();
        return;
    }

    QString h5Name = QString::fromStdString(pt.get<std::string>("level.<xmlattr>.resources"));
    QFileInfo xmlInfo(absoluteLevelPath);
    QDir xmlDir = xmlInfo.dir();

    QString h5Path = xmlDir.absoluteFilePath(h5Name);
    qDebug() << "HDF5 path:" << h5Path;

    std::string texDataset;
    // get tileset of collision tiles
    for (const auto &v : pt.get_child("level"))
        if (v.first == "collision_tiles")
            texDataset = v.second.get("<xmlattr>.texture", "");

    if (texDataset.empty())
    {
        qWarning() << "no texture found";
        return;
    }

    // open hdf5 and  extract tiles
    LevelHdf5IO io;
    QImage tilesetImage;
    try
    {
        io.open(h5Path.toStdString());
        std::vector<size_t> dims;
        auto pixels = io.loadArray<unsigned char>("textures", texDataset, dims);

        int H = static_cast<int>(dims[0]);
        int W = static_cast<int>(dims[1]);
        tilesetImage = QImage(W, H, QImage::Format_ARGB32);
        std::memcpy(tilesetImage.bits(), pixels.get(), H * W * 4);

        // remove transparency
        QImage img = tilesetImage.convertToFormat(QImage::Format_ARGB32);
        int tol = 10;
        for (int y = 0; y < img.height(); y++)
            for (int x = 0; x < img.width(); x++)
            {
                QRgb px = img.pixel(x, y);
                if (qAbs(qRed(px) - 92) <= tol && qAbs(qGreen(px) - 130) <= tol && qAbs(qBlue(px) - 161) <= tol)
                    img.setPixel(x, y, qRgba(0, 0, 0, 0));
            }
        tilesetImage = img;

        // get tiles
        QList<Tile> tiles;
        int totalTileW = tileWidth + offset;
        int totalTileH = tileHeight + offset;
        int cols = (tilesetImage.width() + offset) / totalTileW;
        int rows = (tilesetImage.height() + offset) / totalTileH;

        int idx = 0;
        for (int row = 0; row < rows; row++)
            for (int col = 0; col < cols; col++)
            {
                Tile t;
                t.index = idx++;
                t.pixmap = QPixmap::fromImage(tilesetImage);

                int x = offset + col * totalTileW;
                int y = offset + row * totalTileH;
                t.sourceRect = QRect(x, y, tileWidth, tileHeight);

                tiles.append(t);
            }

        qDebug() << "Tiles extracted:" << tiles.size();

        // give tiles to canvas and palette
        if (m_canvas)
        {
            m_canvas->setTileset(tiles, tileWidth, tileHeight, offset, endIndex);
        }
        if (m_palette)
        {
            m_palette->setTileset(tiles, tileWidth, tileHeight, offset, endIndex);
        }

        m_tilesetPath = h5Path;
        m_tileWidth = tileWidth;
        m_tileHeight = tileHeight;
        m_tileOffset = offset;
        m_endIndex = endIndex;

        emit tilesetPathChanged();
        qDebug() << "Tileset successfully loaded";
    }
    catch (const std::exception &e)
    {
        qWarning() << "Failed to load tileset:" << e.what();
    }
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

    // default save location
    QString defaultPath = QDir::currentPath() + "/res/level_master.xml";
    qDebug() << "Default path:" << defaultPath;

    QString file_name = QFileDialog::getSaveFileName(
        nullptr,
        "Save Level",
        defaultPath,
        "Level Files (*.xml);;All Files (*)");

    if (file_name.isEmpty())
    {
        qDebug() << "Save cancelled";
        return;
    }

    qDebug() << "Saving to:" << file_name;

    // add .xml if needed
    if (!file_name.endsWith(".xml", Qt::CaseInsensitive))
    {
        file_name += ".xml";
    }

    // create dir if missing
    QFileInfo fileInfo(file_name);
    QDir().mkpath(fileInfo.absolutePath());

    // test write access
    QFile file(file_name);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        qWarning() << "Can't write to:" << file_name;
        qWarning() << "Error:" << file.errorString();
        return;
    }
    file.close();

    m_canvas->saveLevel(file_name);

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

    // qDebug() << "Controller: Tile" << tileIndex << "selected";

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
