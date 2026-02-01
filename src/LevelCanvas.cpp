#include "include/LevelCanvas.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QImage>
#include <QFile>
#include <QDir>
#include <QDataStream>
#include <QFileInfo>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QPair>
#include <QMap>
#include <QDebug>

#include <vector>
#include <algorithm> // std::copy
#include <exception> // std::exception

template <typename T>
static jumper::shared_array<T> makeSharedArrayCopy(const std::vector<T> &v)
{
    jumper::shared_array<T> arr(new T[v.size()]);
    std::copy(v.begin(), v.end(), arr.get());
    return arr;
}

LevelCanvas::LevelCanvas(QQuickItem *parent)
    : QQuickPaintedItem(parent), m_backgroundColor(92, 130, 161)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
}

void LevelCanvas::setTileset(const QList<Tile> &tiles, int tileW, int tileH, int offset, int endIndex)
{
    m_tiles = tiles;
    m_tileWidth = tileW;
    m_tileHeight = tileH;
    m_endIndex = endIndex;
    m_tileOffset = offset;

    update();
    qDebug() << "Canvas tileset loaded:" << m_tiles.size() << "tiles";
    qDebug() << "==============================";

    // Initialize frame after tileset is loaded (only if empty)
    if (m_levelData.isEmpty())
    {
        clearLevel(); // Creates the frame
    }
}

void LevelCanvas::placeTile(int tileIndex)
{
    m_currentTileIndex = tileIndex;
    qDebug() << "Current tile set:" << tileIndex;
}

void LevelCanvas::clearLevel()
{
    // Only remove inner tiles, keep frame
    QList<QPair<int, int>> toRemove;
    for (auto it = m_levelData.constBegin(); it != m_levelData.constEnd(); ++it)
    {
        if (!isFrameTile(it.key().first, it.key().second))
        {
            toRemove.append(it.key());
        }
    }
    for (const auto &key : toRemove)
    {
        m_levelData.remove(key);
    }

    // Create frame on first call only
    if (m_levelData.isEmpty())
    {
        for (int x = 1; x < m_gridWidth - 1; x++)
            m_levelData[QPair<int, int>(x, m_gridHeight - 1)] = 1;
        for (int y = 0; y < m_gridHeight; y++)
        {
            m_levelData[QPair<int, int>(0, y)] = 55;
            m_levelData[QPair<int, int>(m_gridWidth - 1, y)] = 55;
        }
    }
    update();
}

void LevelCanvas::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    painter->fillRect(boundingRect(), Qt::white);

    // Draw placed tiles
    for (auto it = m_levelData.constBegin(); it != m_levelData.constEnd(); ++it)
    {
        int gridX = it.key().first;
        int gridY = it.key().second;
        int tileIndex = it.value();

        if (tileIndex >= 0 && tileIndex < m_tiles.size())
        {
            // Draw tile in grid cell
            QRect targetRect(gridX * m_tileWidth,
                             gridY * m_tileHeight,
                             m_tileWidth,
                             m_tileHeight);

            // Cut only the INNER 34x34 pixels from the 36x36 sourceRect
            // (i.e. skip the 2px spacing/border)
            QRect actualSourceRect = m_tiles[tileIndex].sourceRect;
            QRect croppedSource(actualSourceRect.x(),
                                actualSourceRect.y(),
                                m_tileWidth,   // Only 34px wide
                                m_tileHeight); // Only 34px tall

            painter->drawPixmap(targetRect, m_tiles[tileIndex].pixmap, croppedSource);
        }
    }

    // Grid as overlay
    painter->setPen(QPen(QColor(200, 200, 200, 60), 1));
    for (int x = 0; x <= m_gridWidth; x++)
    {
        painter->drawLine(x * m_tileWidth, 0, x * m_tileWidth, m_gridHeight * m_tileHeight);
    }
    for (int y = 0; y <= m_gridHeight; y++)
    {
        painter->drawLine(0, y * m_tileHeight, m_gridWidth * m_tileWidth, y * m_tileHeight);
    }
}

void LevelCanvas::mousePressEvent(QMouseEvent *event)
{
    int gridX = event->position().x() / m_tileWidth;
    int gridY = event->position().y() / m_tileHeight;

    if (gridX >= 0 && gridX < m_gridWidth && gridY >= 0 && gridY < m_gridHeight)
    {
        // Protect frame tiles
        if (isFrameTile(gridX, gridY) && event->button() == Qt::RightButton)
        {
            return; // Can't delete frame tiles
        }

        QPair<int, int> pos(gridX, gridY);

        if (m_levelData.contains(pos))
        {
            // Right click or placing Tile on extraTile (1x2 Tile)
            if (event->button() == Qt::RightButton || (m_levelData[pos] != m_currentTileIndex && m_levelData[pos] >= m_endIndex))
            {
                // if extraTile
                if (m_levelData[pos] >= m_endIndex)
                {
                    // is Upper or Lower Part
                    if (m_levelData[pos] % 2 != m_endIndex % 2)
                    {
                        QPair<int, int> pos2(gridX, gridY - 1);
                        m_levelData.remove(pos2);
                        qDebug() << "Tile deleted at (" << gridX << "," << gridY - 1 << ")";
                    }
                    else
                    {
                        QPair<int, int> pos2(gridX, gridY + 1);
                        m_levelData.remove(pos2);
                        qDebug() << "Tile deleted at (" << gridX << "," << gridY + 1 << ")";
                    }
                }

                m_levelData.remove(pos);
                update();
                qDebug() << "Tile deleted at (" << gridX << "," << gridY << ")";
            }
        }
        else if (event->button() == Qt::LeftButton)
        {
            // Left click: Place tile
            if (m_currentTileIndex < 0)
            {
                qDebug() << "No tile selected!";
                return;
            }
            // if extraTile
            if (m_currentTileIndex >= m_endIndex)
            {
                // check if Upper or Lower and not outside of grid
                if (m_currentTileIndex % 2 != m_endIndex % 2 && gridY != 0)
                {
                    QPair<int, int> pos2(gridX, gridY - 1);
                    // special case: extraTile on extraTile
                    if (m_levelData.contains(pos2))
                    {
                        if (m_levelData[pos2] >= m_endIndex && m_levelData[pos2] % 2 != m_endIndex % 2)
                        {
                            QPair<int, int> pos3(gridX, gridY - 2);
                            m_levelData.remove(pos3);
                        }
                    }
                    m_levelData[pos2] = m_currentTileIndex - 1;
                }
                // check if Upper or Lower and not outside of grid
                else if (m_currentTileIndex % 2 == m_endIndex % 2 && gridY != m_gridHeight - 1)
                {
                    QPair<int, int> pos2(gridX, gridY + 1);
                    // special case: extraTile on extraTile
                    if (m_levelData.contains(pos2))
                    {
                        if (m_levelData[pos2] >= m_endIndex && m_levelData[pos2] % 2 == m_endIndex % 2)
                        {
                            QPair<int, int> pos3(gridX, gridY + 2);
                            m_levelData.remove(pos3);
                        }
                    }
                    m_levelData[pos2] = m_currentTileIndex + 1;
                }
                // tried placing extraTile outside
                else
                {
                    return;
                }
            }
            m_levelData[pos] = m_currentTileIndex;

            update();
            qDebug() << "Tile placed at (" << gridX << "," << gridY << ") with index" << m_currentTileIndex;
        }
    }
}

// -----------------------------------------------
// Helper: flatten a QMap<QPair<int,int>, int> into row-major vector<int>
// defaultValue is used for empty tiles.
// -----------------------------------------------
static std::vector<int> flattenTileMap(
    const QMap<QPair<int, int>, int> &map,
    int w, int h,
    int defaultValue = 0)
{
    std::vector<int> out(w * h, defaultValue);

    for (auto it = map.begin(); it != map.end(); ++it)
    {
        const int x = it.key().first;
        const int y = it.key().second;
        if (x < 0 || x >= w || y < 0 || y >= h)
            continue;
        out[y * w + x] = it.value() + 1;
    }

    return out;
}

// -----------------------------------------------
// Helper: rebuild QMap from row-major vector
// -----------------------------------------------
static QMap<QPair<int, int>, int> unflattenTileMap(
    const std::vector<int> &flat,
    int w, int h,
    int defaultValue = 0)
{
    QMap<QPair<int, int>, int> out;

    if ((int)flat.size() != w * h)
        return out;

    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            const int v = flat[y * w + x] - 1;
            if (v >= 0)
                out.insert(qMakePair(x, y), v);
        }
    }

    return out;
}

// -----------------------------------------------
// Helper: save QImage RGBA8888 into a vector<uint8_t>
// -----------------------------------------------
static bool imageToRgbaBytes(const QImage &image, std::vector<unsigned char> &outBytes, int &H, int &W)
{
    if (image.isNull())
    {
        return false;
    }
    // start with argb
    QImage img = image.convertToFormat(QImage::Format_ARGB32);
    H = img.height();
    W = img.width();
    outBytes.resize(H * W * 4);

    const unsigned char *src = img.bits();
    unsigned char *dst = outBytes.data();
    // Qt to SDL fix by manually switching rgba values
    for (int i = 0; i < H * W; ++i)
    {
        unsigned char a = src[4*i + 3];
        unsigned char r = src[4*i + 2];
        unsigned char g = src[4*i + 1];
        unsigned char b = src[4*i + 0];

        dst[4*i + 0] = b; 
        dst[4*i + 1] = g; 
        dst[4*i + 2] = r; 
        dst[4*i + 3] = a; 

    }

    return true;
}

// -----------------------------------------------
// Helper: convert RGBA bytes back to QImage
// -----------------------------------------------
static QImage rgbaBytesToImage(const std::vector<unsigned char> &bytes, int H, int W)
{
    if ((int)bytes.size() != H * W * 4)
    {
        return QImage();
    }
    // start with argb
    QImage img(W, H, QImage::Format_ARGB32);
    unsigned char *dst = img.bits();

    for (int i = 0; i < H * W; ++i)
    {
        // same fix as above
        unsigned char b = bytes[4*i + 0];
        unsigned char g = bytes[4*i + 1];
        unsigned char r = bytes[4*i + 2];
        unsigned char a = bytes[4*i + 3];

        dst[4*i + 0] = b;
        dst[4*i + 1] = g;
        dst[4*i + 2] = r;
        dst[4*i + 3] = a;
    }

    return img;
}

// ============================================================================
// SAVE LEVEL (XML + H5)
// background_tiles = m_levelData -> /tiles/level2
// collision_tiles  = m_collisionData -> /tiles/level1
// tileset texture  = /textures/<tilesetTextureName>
void LevelCanvas::saveLevel(const QString &xmlPath)
{
    // ---------- path handling ----------
    QString p = xmlPath;
    if (p.startsWith("~"))
        p.replace(0, 1, QDir::homePath());

    QFileInfo xmlInfo(p);
    const QString outDir = xmlInfo.absolutePath();
    const QString baseName = xmlInfo.completeBaseName();
    const QString h5Path = outDir + "/" + baseName + ".h5";

    // ---------- collision data ----------
    const int gridH = 32;
    const int gridW = 73;

    std::vector<int> flatCollision =
        flattenTileMap(m_levelData, gridW, gridH, 0);

    std::vector<size_t> tileDim = {
        static_cast<size_t>(gridH),
        static_cast<size_t>(gridW)};

    // ---------- prepare tileset image ----------
    if (!m_tilesetPath.isEmpty())
    {
                        std::cout << "\ntileset saved\n";
        QImage img(m_tilesetPath);
        if (!img.isNull())
                        std::cout << "\ntileset saved\n";
            // TODO: Change the Color format pls
            m_tilesetImage = img.convertToFormat(QImage::Format_RGBA8888);
    }

    // ---------- prepare background image ----------
    QImage bgImg(":/resources/images/backgroundMountain.png");
    if (!bgImg.isNull())
    {
        bgImg = bgImg.convertToFormat(QImage::Format_RGBA8888);
    }

    QImage iconImg(":/resources/images/heart3.png");
    if (!iconImg.isNull())
    {
        iconImg = iconImg.convertToFormat(QImage::Format_RGBA8888);
    }

    QImage ghostImg(":/resources/images/ghost.png");
    if (!ghostImg.isNull())
    {
        ghostImg = ghostImg.convertToFormat(QImage::Format_RGBA8888);
    }
    QImage snakeImg(":/resources/images/snake.png");
    if (!snakeImg.isNull())
    {
        snakeImg = snakeImg.convertToFormat(QImage::Format_RGBA8888);
    }

    const QString actorResourcePath = ":/resources/images/mario1.png";
    const QString actorTextureName = "mario1";

    QImage actorImg(actorResourcePath);
    if (!actorImg.isNull())
        actorImg = actorImg.convertToFormat(QImage::Format_RGBA8888);

    try
    {
        // ==========================================================
        //  CRTP IO: ONLY TileSetIO
        // ==========================================================
        using IO = jumper::BaseHdf5IO<jumper::hdf5features::TileSetIO>;
        IO io;
        io.open(h5Path.toStdString());

        // ----------------------------------------------------------
        // Save tileset texture as RAW DATASET
        // ----------------------------------------------------------
        if (!m_tilesetImage.isNull())
        {
            std::vector<unsigned char> bytes;
            int h = 0, w = 0;
            if (imageToRgbaBytes(m_tilesetImage, bytes, h, w))
            {
            
                std::vector<size_t> dims = {
                    static_cast<size_t>(h),
                    static_cast<size_t>(w),
                    static_cast<size_t>(4)};

                std::vector<hsize_t> chunks = {
                    dims[0], dims[1], dims[2]};

                auto arr = makeSharedArrayCopy(bytes);

                io.save(
                    "textures",
                    m_tilesetTextureName.toStdString(),
                    dims,
                    chunks,
                    arr);
            }
        }

        // ----------------------------------------------------------
        // Save background texture as RAW DATASET
        // ----------------------------------------------------------
        if (!bgImg.isNull())
        {
            std::vector<unsigned char> bytes;
            int h = 0, w = 0;

            if (imageToRgbaBytes(bgImg, bytes, h, w))
            {
                std::vector<size_t> dims = {
                    static_cast<size_t>(h),
                    static_cast<size_t>(w),
                    static_cast<size_t>(4)};

                std::vector<hsize_t> chunks = {
                    dims[0], dims[1], dims[2]};

                auto arr = makeSharedArrayCopy(bytes);

                io.save(
                    "textures",
                    "backgroundImg",
                    dims,
                    chunks,
                    arr);
            }
        }

        if (!iconImg.isNull())
        {
            std::vector<unsigned char> bytes;
            int h = 0, w = 0;

            if (imageToRgbaBytes(iconImg, bytes, h, w))
            {
                std::vector<size_t> dims = {
                    static_cast<size_t>(h),
                    static_cast<size_t>(w),
                    static_cast<size_t>(4)};

                std::vector<hsize_t> chunks = {
                    dims[0], dims[1], dims[2]};

                auto arr = makeSharedArrayCopy(bytes);

                io.save(
                    "textures",
                    "icons",
                    dims,
                    chunks,
                    arr);
            }
        }

        // ----------------------------------------------------------
        // Save actor texture as RAW DATASET (engine asset)
        // ----------------------------------------------------------
        if (!actorImg.isNull())
        {
            std::vector<unsigned char> bytes;
            int h = 0, w = 0;

            if (imageToRgbaBytes(actorImg, bytes, h, w))
            {
                std::vector<size_t> dims = {
                    static_cast<size_t>(h),
                    static_cast<size_t>(w),
                    static_cast<size_t>(4)};

                std::vector<hsize_t> chunks = {
                    dims[0], dims[1], dims[2]};

                auto arr = makeSharedArrayCopy(bytes);

                io.save(
                    "textures",
                    actorTextureName.toStdString(),
                    dims,
                    chunks,
                    arr);

                qDebug() << "[LevelCanvas] Saved actor texture to H5:"
                         << "/textures/" << actorTextureName
                         << "dim=" << h << "x" << w << "x4";
            }
        }

        // ----------------------------------------------------------
        // Save ghost texture as RAW DATASET (engine asset)
        // ----------------------------------------------------------
        if (!ghostImg.isNull())
        {
            std::vector<unsigned char> bytes;
            int h = 0, w = 0;

            if (imageToRgbaBytes(ghostImg, bytes, h, w))
            {
                std::vector<size_t> dims = {
                    static_cast<size_t>(h),
                    static_cast<size_t>(w),
                    static_cast<size_t>(4)};

                std::vector<hsize_t> chunks = {
                    dims[0], dims[1], dims[2]};

                auto arr = makeSharedArrayCopy(bytes);

                io.save(
                    "textures",
                    "ghost",
                    dims,
                    chunks,
                    arr);
            }
        }

        // ----------------------------------------------------------
        // Save snake texture as RAW DATASET (engine asset)
        // ----------------------------------------------------------
        if (!snakeImg.isNull())
        {
            std::vector<unsigned char> bytes;
            int h = 0, w = 0;

            if (imageToRgbaBytes(snakeImg, bytes, h, w))
            {
                std::vector<size_t> dims = {
                    static_cast<size_t>(h),
                    static_cast<size_t>(w),
                    static_cast<size_t>(4)};

                std::vector<hsize_t> chunks = {
                    dims[0], dims[1], dims[2]};

                auto arr = makeSharedArrayCopy(bytes);

                io.save(
                    "textures",
                    "snake",
                    dims,
                    chunks,
                    arr);

                qDebug() << "[LevelCanvas] Saved actor texture to H5:"
                         << "/textures/" << actorTextureName
                         << "dim=" << h << "x" << w << "x4";
            }
        }

        // ----------------------------------------------------------
        // Save collision tiles (TileSetIO responsibility)
        // ----------------------------------------------------------
        auto colArr = makeSharedArrayCopy(flatCollision);

        io.save<int>(
            "tiles",
            "level1",
            tileDim,
            colArr);
    }
    catch (const std::exception &e)
    {
        qWarning() << "[LevelCanvas] HDF5 save failed:" << e.what();
        return;
    }

    // ---- XML SAVE ----
    QFile f(p);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning() << "[LevelCanvas] Could not write XML:" << p;
        return;
    }

    QTextStream ts(&f);

    int tilesPerRow = 0;
    int numRows = 0;
    if (!m_tilesetImage.isNull() && m_tileWidth > 0 && m_tileHeight > 0)
    {
        tilesPerRow = m_tilesetImage.width() / (m_tileWidth + m_tileOffset);
        numRows = m_tilesetImage.height() / (m_tileHeight + m_tileOffset);
    }

    ts << "<level resources=\"" << baseName + ".h5" << "\">\n";

    // background still references the tileset for tilemap usage
    // (If you want XML to reference the PNG instead, tell me and I adapt)
    ts << "  <background_tiles texture=\"" << "backgroundImg" << "\">\n";
    ts << "    <layer>0</layer>\n";
    ts << "  </background_tiles>\n";

    ts << "  <collision_tiles texture=\"" << m_tilesetTextureName << "\" tiles=\"level1\">\n";
    ts << "    <tileWidth>" << m_tileWidth << "</tileWidth>\n";
    ts << "    <tileHeight>" << m_tileHeight << "</tileHeight>\n";
    ts << "    <tilesPerRow>" << tilesPerRow << "</tilesPerRow>\n";
    ts << "    <numRows>" << numRows << "</numRows>\n";
    ts << "    <tileOffset>" << m_tileOffset << "</tileOffset>\n";
    ts << "    <switchIndex>" << m_endIndex << "</switchIndex>\n";
    ts << "    <layer>1</layer>\n";
    ts << "  </collision_tiles>\n";

    // icons like heart
    ts << "  <icons texture=\"icons\">\n";
    ts << "    <tileWidth>" << 32 << "</tileWidth>\n";
    ts << "    <tileHeight>" << 32 << "</tileHeight>\n";
    ts << "    <layer>3</layer>\n";
    ts << "  </icons>\n";
    // Actor block (Dummy values for now) //TODO: if there are actor settings to save
    // then this should be adapted and changed accordingly
    ts << "  <actor texture=\"" << actorTextureName << "\">\n";
    ts << "    <num_frames>2</num_frames>\n";
    ts << "    <frame_width>18</frame_width>\n";
    ts << "    <frame_height>32</frame_height>\n";
    ts << "    <position_x>50</position_x>\n";
    ts << "    <position_y>1350</position_y>\n";
    ts << "    <layer>2</layer>\n";
    ts << "  </actor>\n";

    // keep forces block
    ts << "  <level_forces>\n";
    ts << "    <gravity_x>0</gravity_x>\n";
    ts << "    <gravity_y>400</gravity_y>\n";
    ts << "    <damping_x>0.7</damping_x>\n";
    ts << "    <damping_y>1.0</damping_y>\n";
    ts << "  </level_forces>\n";

    ts << "</level>\n";
    f.close();

    qDebug() << "[LevelCanvas] XML saved:" << p;
    qDebug() << "[LevelCanvas] H5 saved:" << h5Path;
    qDebug() << "[LevelCanvas] Done.";
}

// ============================================================================
// LOAD LEVEL (XML + H5)
// Reads xml -> finds texture name + tiles dataset names
// Loads H5 datasets into:
//   m_tilesetImage (from /textures/<texture>)
//   m_collisionData (from /tiles/<level1>)
//   m_levelData (from /tiles/<level2>)
// ============================================================================
void LevelCanvas::loadLevel(const QString &xmlPath)
{
    qDebug() << "[LevelCanvas] loadLevel ->" << xmlPath;

    // expand ~
    QString p = xmlPath;
    if (p.startsWith("~"))
        p.replace(0, 1, QDir::homePath());

    QFileInfo xmlInfo(p);
    if (!xmlInfo.exists())
    {
        qWarning() << "[LevelCanvas] XML file does not exist:" << p;
        return;
    }

    QString h5FileName;
    QString bgTextureName;
    QString colTextureName;
    QString colTilesDataset;

    // -------- parse XML (simple line-based) --------
    QFile f(p);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "[LevelCanvas] Could not open XML:" << p;
        return;
    }

    QXmlStreamReader xml(&f);
    
    while (!xml.atEnd())
    {
        xml.readNext();

        if (xml.isStartElement())
        {
            QString elementName = xml.name().toString();

            if (elementName == "level")
            {
                h5FileName = xml.attributes().value("resources").toString();
            }
            else if (elementName == "background_tiles")
            {
                bgTextureName = xml.attributes().value("texture").toString();
            }
            else if (elementName == "collision_tiles")
            {
                colTextureName = xml.attributes().value("texture").toString();
                colTilesDataset = xml.attributes().value("tiles").toString();
            }
            else if (elementName == "tileWidth")
            {
                bool ok = false;
                int v = xml.readElementText().toInt(&ok);
                if (ok)
                    m_tileWidth = v;
            }
            else if (elementName == "tileHeight")
            {
                bool ok = false;
                int v = xml.readElementText().toInt(&ok);
                if (ok)
                    m_tileHeight = v;
            }
            else if (elementName == "tileOffset")
            {
                bool ok = false;
                int v = xml.readElementText().toInt(&ok);
                if (ok)
                    m_tileOffset = v;
            }
        }
    }

    if (xml.hasError())
    {
        qWarning() << "[LevelCanvas] XML parsing error:" << xml.errorString();
    }
    f.close();

    if (h5FileName.isEmpty())
    {
        qWarning() << "[LevelCanvas] XML has no resources=\"...\" attribute.";
        return;
    }

    // Prefer collision texture name, otherwise background texture name
    m_tilesetTextureName = !colTextureName.isEmpty() ? colTextureName : bgTextureName;

    if (colTilesDataset.isEmpty())
        colTilesDataset = "level1";

    // -------- build H5 path next to XML --------
    const QString h5Path = xmlInfo.absolutePath() + "/" + h5FileName;

    // -------- H5 LOAD via CRTP BaseHdf5IO (TileSetIO ONLY) --------
    try
    {
        using IO = jumper::BaseHdf5IO<jumper::hdf5features::TileSetIO>;
        IO io;
        io.open(h5Path.toStdString());

        // ----------------------------------------------------------
        // Load collision tiles (raw tileset data)
        // ----------------------------------------------------------
        std::vector<size_t> tileDims;
        auto tileArr =
            io.TileSetIO::loadRaw("tiles", colTilesDataset.toStdString(), tileDims);

        if (tileDims.size() < 2)
        {
            qWarning() << "[LevelCanvas] Invalid tile dataset dimensions.";
            return;
        }

        m_gridHeight = static_cast<int>(tileDims[0]);
        m_gridWidth = static_cast<int>(tileDims[1]);
        // flatten TileArray<T> into editor map
        std::vector<int> flatTiles;
        flatTiles.reserve(m_gridWidth * m_gridHeight);

        for (int y = 0; y < m_gridHeight; ++y)
            for (int x = 0; x < m_gridWidth; ++x)
                flatTiles.push_back(tileArr[y][x]);

        m_levelData = unflattenTileMap(flatTiles, m_gridWidth, m_gridHeight, 0);

        // ----------------------------------------------------------
        // Load tileset texture as RAW ARRAY (no TextureIO!)
        // ----------------------------------------------------------
        if (!m_tilesetTextureName.isEmpty())
        {
            std::vector<size_t> texDims;
            auto texArr =
                io.loadArray<unsigned char>(
                    "textures",
                    m_tilesetTextureName.toStdString(),
                    texDims);

            if (texDims.size() != 3 || texDims[2] != 4)
            {
                qWarning() << "[LevelCanvas] Unexpected texture dimensions.";
                return;
            }

            int H = static_cast<int>(texDims[0]);
            int W = static_cast<int>(texDims[1]);

            std::vector<unsigned char> texBytes(H * W * 4);
            std::memcpy(texBytes.data(), texArr.get(), texBytes.size());

            m_tilesetImage = rgbaBytesToImage(texBytes, H, W);
        }
    }
    catch (const std::exception &e)
    {
        qWarning() << "[LevelCanvas] H5 load failed:" << e.what();
        return;
    }

    update();

    qDebug() << "[LevelCanvas] Loaded level:"
             << "grid=" << m_gridWidth << "x" << m_gridHeight
             << "texture=" << m_tilesetTextureName;
}

// in 1x2 Tiles / extraTiles Mode
void LevelCanvas::setExtraTiles(bool mode)
{
    m_extraTiles = mode;
}