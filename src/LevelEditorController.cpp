#include "include/LevelEditorController.hpp"
#include "include/TilesetPalette.hpp"
#include "include/LevelCanvas.hpp"
#include <QDebug>
#include <QFileDialog>
#include <QXmlStreamReader>
#include "game/io/BaseHdf5IO.hpp"
#include "game/io/TileSetIO.hpp"
#include "game/include/Util.hpp"

using LevelHdf5IO = jumper::BaseHdf5IO<jumper::hdf5features::TileSetIO>;

LevelEditorController::LevelEditorController(QObject *parent)
    : QObject(parent)
{
    qDebug() << "[LevelEditorController] Created";
}

void LevelEditorController::registerPalette(TilesetPalette *palette)
{
    if (m_palette != nullptr)
    {
        qWarning() << "[LevelEditorController] Palette already registered, replacing...";
    }

    m_palette = palette;
    // connect signals for selection
    connect(m_palette, &TilesetPalette::tileSelected,
            this, &LevelEditorController::selectTile);

    // connect signals for  tileset loading
    connect(this, &LevelEditorController::tilesetReady,
            m_palette, &TilesetPalette::setTileset);

    qDebug() << "[LevelEditorController] TilesetPalette registered with controller";
}

void LevelEditorController::registerCanvas(LevelCanvas *canvas)
{
    if (m_canvas != nullptr)
    {
        qWarning() << "[LevelEditorController] Canvas already registered, replacing...";
    }

    m_canvas = canvas;

    // connect signal tileset loading
    connect(this, &LevelEditorController::tilesetReady,
            m_canvas, &LevelCanvas::setTileset);

    qDebug() << "[LevelEditorController] LevelCanvas registered with controller";
}

void LevelEditorController::loadTileset(const QString &path)
{

    // Load tileset into both palette and canvas
    if (m_palette)
    {
        //m_palette->loadTileset(path, tileWidth, tileHeight, offset, endIndex);
        // Load the Tile-names from the XML file
        m_palette->loadTileNames("res/tileDefinition/RulesTiles.xml");
        qDebug() << "[LevelEditorController] Tileset loaded into palette";
    }

    QString absoluteLevelPath = path;
    if (absoluteLevelPath.isEmpty()) {
        QDir appDir(QCoreApplication::applicationDirPath());
        appDir.cdUp();
        absoluteLevelPath = appDir.absoluteFilePath("res/level.xml");

        if (!QFile::exists(absoluteLevelPath)) {
            QDir currentDir = QDir::current();
            absoluteLevelPath = currentDir.absoluteFilePath("res/level.xml");
        }
    } else if (!QDir::isAbsolutePath(absoluteLevelPath)) {
        QDir appDir(QCoreApplication::applicationDirPath());
        appDir.cdUp();
        absoluteLevelPath = appDir.absoluteFilePath(absoluteLevelPath);

        if (!QFile::exists(absoluteLevelPath)) {
            QDir currentDir = QDir::current();
            absoluteLevelPath = currentDir.absoluteFilePath(path);
        }
    }

    // get full xml path
    QString xmlPath = absoluteLevelPath;

    // read xml
    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QFile::ReadOnly | QFile::Text))
    {
        qWarning() << "[LevelEditorController] Failed to read XML:" << xmlPath;
        return;
    }

    QXmlStreamReader xml(&xmlFile);
    QString h5Name;
    std::string texDataset;

    while (!xml.atEnd())
    {
        xml.readNext();

        if (xml.isStartElement())
        {
            if (xml.name() == QString("level"))
            {
                h5Name = xml.attributes().value("resources").toString();
            }
            else if (xml.name() == QString("collision_tiles"))
            {
                texDataset = xml.attributes().value("texture").toString().toStdString();
            }
            else if (xml.name() == QString("tileWidth"))
            {
                m_tileWidth = xml.readElementText().toInt();
            }
            else if (xml.name() == QString("tileHeight"))
            {
                m_tileHeight = xml.readElementText().toInt();
            }
            else if (xml.name() == QString("tileOffset"))
            {
                m_tileOffset = xml.readElementText().toInt();
            }
            else if (xml.name() == QString("switchIndex"))
            {
                m_endIndex = xml.readElementText().toInt();
            }
        }
    }

    if (xml.hasError())
    {
        qWarning() << "[LevelEditorController] XML parsing error:" << xml.errorString();
        xmlFile.close();
        return;
    }
    xmlFile.close();

    if (texDataset.empty())
    {
        qWarning() << "[LevelEditorController] No texture found";
        return;
    }

    QFileInfo xmlInfo(absoluteLevelPath);
    QDir xmlDir = xmlInfo.dir();

    QString h5Path = xmlDir.absoluteFilePath(h5Name);
    qDebug() << "[LevelEditorController] HDF5 path:" << h5Path;
    m_tilesetPath = h5Path;

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
                if (qAbs(qRed(px) - 94) <= tol && qAbs(qGreen(px) - 129) <= tol && qAbs(qBlue(px) - 162) <= tol)
                {
                    img.setPixel(x, y, qRgba(0, 0, 0, 0));
                }
            }
        tilesetImage = img;

        // get tiles
        QList<Tile> tiles;
        int totalTileW = m_tileWidth + m_tileOffset;
        int totalTileH = m_tileHeight + m_tileOffset;
        int cols = (tilesetImage.width() + m_tileOffset) / totalTileW;
        int rows = (tilesetImage.height() + m_tileOffset) / totalTileH;

        int idx = 0;
        for (int row = 0; row < rows; row++)
            for (int col = 0; col < cols; col++)
            {
                Tile t;
                t.index = idx++;
                t.pixmap = QPixmap::fromImage(tilesetImage);

                int x = m_tileOffset + col * totalTileW;
                int y = m_tileOffset + row * totalTileH;
                t.sourceRect = QRect(x, y, m_tileWidth, m_tileHeight);

                tiles.append(t);
            }

        qDebug() << "[LevelEditorController] Tiles extracted:" << tiles.size();

        // give tiles to canvas and palette
        if (m_canvas)
        {
            m_canvas->setTileset(tiles, m_tileWidth, m_tileHeight, m_tileOffset, m_endIndex);
        }
        if (m_palette)
        {
            m_palette->setTileset(tiles, m_tileWidth, m_tileHeight, m_tileOffset, m_endIndex);
        }

        emit tilesetPathChanged();
        qDebug() << "[LevelEditorController] Tileset successfully loaded";
    }
    catch (const std::exception &e)
    {
        qWarning() << "[LevelEditorController] Failed to load tileset:" << e.what();
    }
}

void LevelEditorController::newLevel(int gridWidth, int gridHeight)
{
    m_gridWidth = gridWidth;
    m_gridHeight = gridHeight;

    qDebug() << "[LevelEditorController] Creating new level:" << gridWidth << "x" << gridHeight;

    if (m_canvas)
    {
        m_canvas->setGridWidth(gridWidth);
        m_canvas->setGridHeight(gridHeight);
        m_canvas->clearLevel();
        qDebug() << "[LevelEditorController] New level created";
    }
    else
    {
        qWarning() << "[LevelEditorController] Canvas not registered!";
    }
}

void LevelEditorController::clearLevel()
{
    qDebug() << "[LevelEditorController] Clearing level...";

    if (m_canvas)
    {
        m_canvas->setGridWidth(m_gridWidth);
        m_canvas->setGridHeight(m_gridHeight);
        m_canvas->clearLevel();
        emit levelCleared();
        qDebug() << "[LevelEditorController] Level cleared";
    }
    else
    {
        qWarning() << "[LevelEditorController] Canvas not registered!";
    }
}

void LevelEditorController::saveLevel()
{
    qDebug() << "[LevelEditorController] Opening save dialog...";

    if (!m_canvas)
    {
        qWarning() << "[LevelEditorController] Canvas not registered!";
        return;
    }

    // default save location
    QString defaultPath = QDir::currentPath() + "/res/level_master.xml";
    qDebug() << "[LevelEditorController] Default path:" << defaultPath;

    QString file_name = QFileDialog::getSaveFileName(
        nullptr,
        "Save Level",
        defaultPath,
        "Level Files (*.xml);;All Files (*)");

    if (file_name.isEmpty())
    {
        qDebug() << "[LevelEditorController] Save cancelled";
        return;
    }

    qDebug() << "[LevelEditorController] Saving to:" << file_name;

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
        qWarning() << "[LevelEditorController] Can't write to:" << file_name;
        qWarning() << "[LevelEditorController] Error:" << file.errorString();
        return;
    }
    file.close();

    m_canvas->saveLevel(file_name);

    emit levelSaved(file_name);
    qDebug() << "[LevelEditorController] Level saved to:" << file_name;
}

void LevelEditorController::loadLevel()
{
    qDebug() << "[LevelEditorController] Opening load dialog...";

    if (!m_canvas)
    {
        qWarning() << "[LevelEditorController] Canvas not registered!";
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
        qDebug() << "[LevelEditorController] Load cancelled by user";
        return;
    }

    // Open file for reading
    QFile file(file_name);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qWarning() << "[LevelEditorController] Could not open file for reading:" << file_name;
        return;
    }

    // Load level
    m_canvas->loadLevel(file_name);
    file.close();

    emit levelLoaded(file_name);
    qDebug() << "[LevelEditorController] Level loaded from:" << file_name;
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
        qWarning() << "[LevelEditorController] Canvas not registered!";
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

// Add Rows to the canvas
void LevelEditorController::addRowsAbove(int rows)
{
    if (!m_canvas || rows <= 0)
        return;

    m_canvas->addRowsAbove(rows);
}