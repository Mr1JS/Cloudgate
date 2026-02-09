#include "include/LevelCanvas.hpp"
#include "include/Util.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QImage>
#include <QFile>
#include <QDir>
#include <QDataStream>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QPair>
#include <QMap>
#include <QDebug>
#include <QDirIterator>

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
    // Change grid to standard values
    m_gridWidth = 20;
    m_gridHeight = 25;

    int floorY = m_gridHeight - 1;

    // left and right + ground
    for (int y = 0; y < m_gridHeight; ++y)
    {
        m_levelData[QPair<int, int>(0, y)] = 84;               // left wall
        m_levelData[QPair<int, int>(m_gridWidth - 1, y)] = 84; // right wall
    }

    // ground
    for (int x = 1; x < m_gridWidth - 1; ++x)
    {
        m_levelData[QPair<int, int>(x, floorY)] = 1;
    }

    // Modify canvas
    setHeight(m_gridHeight * m_tileHeight);
    setWidth(m_gridWidth * m_tileWidth);

    // set background image to base value
    setBackground("res/images/backgrounds/mountain.png");
    
    emit gridHeightChanged();
    emit gridWidthChanged();
    update();
}

void LevelCanvas::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

    painter->fillRect(boundingRect(), Qt::transparent);

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

        // player spawnpoint marked red
        painter->setBrush(QColor(255, 0, 0, 60));
        painter->setPen(QPen(QColor(255, 0, 0, 180), 2));

        // because of the borders of the game, it doesnt start at (0|max)
        for (int y = m_gridHeight - 3; y < m_gridHeight - 1; ++y)
        {
            for (int x = 1; x < 3; ++x)
            {
                QRect r(x * m_tileWidth,
                        y * m_tileHeight,
                        m_tileWidth,
                        m_tileHeight);

                painter->drawRect(r);
            }
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

    // 2x2 player spawnpoint in grid, so no placing blocks allowed
    if (gridX >= 1 && gridX < 3 && gridY >= m_gridHeight - 3 && gridY < m_gridHeight - 1)
    {
        qDebug() << "Character Spawnpoint - click ignored";
        return;
    }

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
// count how many tiles have coins in them
static int countCoins(const QMap<QPair<int, int>, int> &map)
{
    int coinsAmount = 0;
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        if (it.value() == 118)
        {
            coinsAmount++;
        }
    }

    return coinsAmount;
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
        unsigned char a = src[4 * i + 3];
        unsigned char r = src[4 * i + 2];
        unsigned char g = src[4 * i + 1];
        unsigned char b = src[4 * i + 0];

        dst[4 * i + 0] = b;
        dst[4 * i + 1] = g;
        dst[4 * i + 2] = r;
        dst[4 * i + 3] = a;
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
        unsigned char b = bytes[4 * i + 0];
        unsigned char g = bytes[4 * i + 1];
        unsigned char r = bytes[4 * i + 2];
        unsigned char a = bytes[4 * i + 3];

        dst[4 * i + 0] = b;
        dst[4 * i + 1] = g;
        dst[4 * i + 2] = r;
        dst[4 * i + 3] = a;
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
    // Changed standard values to the actual values of the Grid
    int gridW = m_gridWidth;
    int gridH = m_gridHeight;

    std::vector<int> flatCollision =
        flattenTileMap(m_levelData, gridW, gridH, 0);

    std::vector<size_t> tileDim = {
        static_cast<size_t>(gridH),
        static_cast<size_t>(gridW)};

    // get name of currently selected actor
    QDir appDir(QCoreApplication::applicationDirPath());
    appDir.cdUp();
    std::string absoluteLevelPath = appDir.absoluteFilePath("res/level_master.xml").toStdString();

    QString currentActorName = QString::fromStdString(jumper::getLevelActor(absoluteLevelPath));
    if (currentActorName == QString(""))
    {
        currentActorName = QString("MariosVergessenerZweiterBruder");
    }

    // get full path for background Image (use appDir of code above)
    QString absoluteBackgroundPath = appDir.absoluteFilePath(m_background);

    // ---------- prepare tileset image ----------
    if (!m_tilesetPath.isEmpty())
    {
        QImage img(m_tilesetPath);
        if (!img.isNull())
        {
            // TODO: Change the Color format pls
            m_tilesetImage = img.convertToFormat(QImage::Format_RGBA8888);
        }
    }

    // ---------- prepare background image ----------
    QImage bgImg(absoluteBackgroundPath);
    if (!bgImg.isNull())
    {
        bgImg = bgImg.convertToFormat(QImage::Format_RGBA8888);
    }

    QImage heartImg(":/resources/images/heart3.png");
    if (!heartImg.isNull())
    {
        heartImg = heartImg.convertToFormat(QImage::Format_RGBA8888);
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

    QImage numberImg(":/resources/images/numbers.png");
    if (!numberImg.isNull())
    {
        numberImg = numberImg.convertToFormat(QImage::Format_RGBA8888);
    }

    QMap<QString, QImage> actorImages;
    // iterate through all (.png) actors in assets.qrc defined
    QDirIterator it(":/resources/images/actors", QStringList() << "*.png", QDir::Files);

    while (it.hasNext())
    {
        QString actor = it.next();
        QFileInfo info(actor);
        QString actorName = info.baseName();

        QImage img(actor);
        if (!img.isNull())
        {
            img = img.convertToFormat(QImage::Format_RGBA8888);
            actorImages.insert(actorName, img);
        }
    }

    try
    {
        // ==========================================================
        //  CRTP IO: ONLY TileSetIO
        // ==========================================================
        using IO = jumper::BaseHdf5IO<jumper::hdf5features::TileSetIO>;
        IO io;
        // TODO: when file is replaced then we have an error:
        /* tileset saved
        qt.gui.imageio: libpng warning: iCCP: known incorrect sRGB profile
        qt.gui.imageio: libpng warning: iCCP: cHRM chunk does not match sRGB

        tileset saved
        [LevelCanvas] Saved actor texture to H5: /textures/ "mario1" dim= 30 x 36 x4
        [LevelCanvas] Saved actor texture to H5: /textures/ "mario1" dim= 50 x 48 x4
        [Hdf5Util - createDataset] WARNING: size has changed. resizing dataset
        HDF5-DIAG: Error detected in HDF5 (1.10.10) thread 1:
        #000: ../../../src/H5D.c line 861 in H5Dset_extent(): unable to set dataset extent
            major: Dataset
            minor: Can't set value
        #001: ../../../src/H5Dint.c line 2801 in H5D__set_extent(): unable to modify size of dataspace
            major: Dataset
            minor: Unable to initialize object
        #002: ../../../src/H5S.c line 1823 in H5S_set_extent(): dimension cannot exceed the existing maximal size (new: 65 max: 40)
            major: Dataspace
            minor: Bad value */
        // otherwise if u create a file for that it works
        // TODO: CURRENT ALTERNATIVE ... NEEDS TO BE INVESTIGATED
        // ----------------------------------------------------------
        // Ensure fresh HDF5 file (avoid file lock + stale datasets
        // ----------------------------------------------------------
        if (QFile::exists(h5Path))
        {
            if (!QFile::remove(h5Path))
            {
                qWarning() << "[LevelCanvas] Could not remove existing H5 file:" << h5Path;
            }
        }

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

        // ----------------------------------------------------------
        // Save hp heart texture as RAW DATASET
        // ----------------------------------------------------------
        if (!heartImg.isNull())
        {
            std::vector<unsigned char> bytes;
            int h = 0, w = 0;

            if (imageToRgbaBytes(heartImg, bytes, h, w))
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
                    "heart",
                    dims,
                    chunks,
                    arr);
            }
        }

        // ----------------------------------------------------------
        // Save number texture as RAW DATASET
        // ----------------------------------------------------------
        if (!numberImg.isNull())
        {
            std::vector<unsigned char> bytes;
            int h = 0, w = 0;

            if (imageToRgbaBytes(numberImg, bytes, h, w))
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
                    "numbers",
                    dims,
                    chunks,
                    arr);
            }
        }

        // ----------------------------------------------------------
        // Save actors textures as RAW DATASET (engine asset)   // iterate through all actors
        // ----------------------------------------------------------
        for (auto it = actorImages.constBegin(); it != actorImages.constEnd(); ++it)
        {
            const QString &actorName = it.key();
            const QImage &actorImg = it.value();

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
                        actorName.toStdString(),
                        dims,
                        chunks,
                        arr);
                }
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
    std::array<int, 3> qmlValues = LevelCanvas::getQMLValues();
    int scrollSpeed = qmlValues[0];
    int goalType = qmlValues[1];
    int goalValue = qmlValues[2];
    // prevent a win condition being collect more coins than possible
    if (goalType == 1)
    {
        int maxCoins = countCoins(m_levelData);
        if (maxCoins < goalValue)
        {
            goalValue = maxCoins;
        }
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
    ts << "    <background_path>" << m_background << "</background_path>\n";
    ts << "    <layer>0</layer>\n";
    ts << "  </background_tiles>\n";

    ts << "  <collision_tiles texture=\"" << m_tilesetTextureName << "\" tiles=\"level1\">\n";
    ts << "    <tileWidth>" << m_tileWidth << "</tileWidth>\n";
    ts << "    <tileHeight>" << m_tileHeight << "</tileHeight>\n";
    ts << "    <tilesPerRow>" << tilesPerRow << "</tilesPerRow>\n";
    ts << "    <numRows>" << numRows << "</numRows>\n";
    ts << "    <gridHeight>" << m_gridHeight << "</gridHeight>\n"; // switch with code from develop branch
    ts << "    <tileOffset>" << m_tileOffset << "</tileOffset>\n";
    ts << "    <switchIndex>" << m_endIndex << "</switchIndex>\n";
    ts << "    <layer>1</layer>\n";
    ts << "  </collision_tiles>\n";

    // hp hearts
    ts << "  <heart texture=\"heart\">\n";
    ts << "    <tileWidth>" << 16 << "</tileWidth>\n";
    ts << "    <tileHeight>" << 16 << "</tileHeight>\n";
    ts << "    <layer>3</layer>\n";
    ts << "  </heart>\n";

    // numbers
    ts << "  <numbers texture=\"numbers\">\n";
    ts << "    <num_frames>" << 10 << "</num_frames>\n";
    ts << "    <frame_width>" << 10 << "</frame_width>\n";
    ts << "    <frame_height>" << 10 << "</frame_height>\n";
    ts << "    <layer>3</layer>\n";
    ts << "  </numbers>\n";


    // fixed values because all actors have the same settings except for name
    ts << "  <actor texture=\"" << currentActorName << "\">\n";
    ts << "    <num_frames>2</num_frames>\n";
    ts << "    <frame_width>36</frame_width>\n";
    ts << "    <frame_height>48</frame_height>\n";
    ts << "    <position_x>50</position_x>\n";
    ts << "    <position_y>450</position_y>\n";
    ts << "    <layer>2</layer>\n";
    ts << "  </actor>\n";

    // keep forces block
    ts << "  <level_forces>\n";
    ts << "    <gravity_x>0</gravity_x>\n";
    ts << "    <gravity_y>400</gravity_y>\n";
    ts << "    <damping_x>0.7</damping_x>\n";
    ts << "    <damping_y>1.0</damping_y>\n";
    ts << "    <scrollSpeed>" << scrollSpeed << "</scrollSpeed>\n";
    ts << "  </level_forces>\n";

    ts << "  <goal>\n";
    ts << "    <type>" << goalType << "</type>\n";
    ts << "    <value>" << goalValue << "</value>\n";
    ts << "  </goal>\n";

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
    int goalType = 0;
    int goalValue = 0;
    int scrollSpeed = 0;

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

                while (!(xml.isEndElement() && xml.name().toString() == "background_tiles"))
                {
                    xml.readNext();
                    if (xml.isStartElement())
                    {
                        QString childName = xml.name().toString();

                        if (childName == "background_path")
                        {
                            QString v = xml.readElementText();
                            if (v != "")
                            {
                                setBackground(v);
                            }
                        }
                    }
                }
                
            }
            else if (elementName == "collision_tiles")
            {
                colTextureName = xml.attributes().value("texture").toString();
                colTilesDataset = xml.attributes().value("tiles").toString();

                while (!(xml.isEndElement() && xml.name().toString() == "collision_tiles"))
                {
                    xml.readNext();
                    if (xml.isStartElement())
                    {
                        QString childName = xml.name().toString();

                        if (childName == "tileWidth")
                        {
                            bool ok = false;
                            int v = xml.readElementText().toInt(&ok);
                            if (ok)
                            {
                                m_tileWidth = v;
                            }
                        }
                        else if (childName == "tileHeight")
                        {
                            bool ok = false;
                            int v = xml.readElementText().toInt(&ok);
                            if (ok)
                            {
                                m_tileHeight = v;
                            }
                        }
                        else if (childName == "tileOffset")
                        {
                            bool ok = false;
                            int v = xml.readElementText().toInt(&ok);
                            if (ok)
                            {
                                m_tileOffset = v;
                            }
                        }
                    }
                }
            }
            else if (elementName == "goal")
            {
                while (!(xml.isEndElement() && xml.name().toString() == "goal"))
                {
                    xml.readNext();
                    if (xml.isStartElement())
                    {
                        QString childName = xml.name().toString();

                        if (childName == "type")
                        {
                            bool ok = false;
                            int v = xml.readElementText().toInt(&ok);
                            if (ok)
                            {
                                goalType = v;
                            }
                        }
                        else if (childName == "value")
                        {
                            bool ok = false;
                            int v = xml.readElementText().toInt(&ok);
                            if (ok)
                            {
                                goalValue = v;
                            }
                        }
                    }
                }
            }
            else if (elementName == "level_forces")
            {
                while (!(xml.isEndElement() && xml.name().toString() == "level_forces"))
                {
                    xml.readNext();
                    if (xml.isStartElement())
                    {
                        QString childName = xml.name().toString();

                        if (childName == "scrollSpeed")
                        {
                            bool ok = false;
                            int v = xml.readElementText().toInt(&ok);
                            if (ok)
                            {
                                scrollSpeed = v;
                            }
                        }
                    }
                }
            }
        }
    }
    // set slider and buttons to values of loaded level
    setQMLValues({scrollSpeed, goalType, goalValue});

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

    setHeight(m_gridHeight * m_tileHeight);
    setWidth(m_gridWidth * m_tileWidth);

    emit gridHeightChanged();
    emit gridWidthChanged();

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

// used every time in this class to change m_background so the emit signal is centralized here
void LevelCanvas::setBackground(QString background)
{
    m_background = background;

    emit backgroundChanged(m_background);
}

QString LevelCanvas::background()
{
    return m_background;
}

void LevelCanvas::setQML(QObject* root)
{
    m_qmlRoot = root;
}

std::array<int, 3> LevelCanvas::getQMLValues()
{
    // basic values
    int scrollValue = 8;
    // type 0 = no condition  type 1 = coins  type 2 = time
    int type = 0;
    bool selected = false;

    // scrollSpeed
    QObject* slider = m_qmlRoot->findChild<QObject*>("scrollSpeed");
    if (slider) 
    {
        scrollValue = slider->property("value").toInt() * 4;
    }

    // no win condition
    QObject* buttonNone = m_qmlRoot->findChild<QObject*>("buttonNone");
    if (buttonNone) 
    {
        selected = buttonNone->property("checked").toBool();
        if (selected)
        {
            type = 0;
            return {scrollValue, type, 0};
        }
    }

    // coin win condition
    QObject* buttonCoins = m_qmlRoot->findChild<QObject*>("buttonCoins");
    if (buttonCoins) 
    {
        selected = buttonCoins->property("checked").toBool();
        if (selected)
        {
            type = 1;
            QObject* coins = m_qmlRoot->findChild<QObject*>("coinInput");
            if (coins) 
            {
                int coinVal = coins->property("text").toInt();
                return {scrollValue, type, coinVal};
            }
        }
    }

    // time win condition
    QObject* buttonTime = m_qmlRoot->findChild<QObject*>("buttonTime");
    if (buttonTime) 
    {
        selected = buttonTime->property("checked").toBool();
        if (selected)
        {
            type = 2;
            QObject* time = m_qmlRoot->findChild<QObject*>("timeInput");
            if (time) 
            {
                int timeVal = time->property("text").toInt();
                return {scrollValue, type, timeVal};
            }
        }
    }
    // if error -> no win condition and base scrollValue
    return {(scrollValue), 0, 0};

}

void LevelCanvas::setQMLValues(std::array<int, 3> qmlValues)
{
    int scrollSpeed = qmlValues[0] / 4;
    int type = qmlValues[1];
    int value = qmlValues[2];

    // scroll slider
    QObject* slider = m_qmlRoot->findChild<QObject*>("scrollSpeed");
    if (slider) 
    {
        slider->setProperty("value", scrollSpeed);
    }

    // no win condition
    if (type == 0)
    {
        QObject* buttonNone = m_qmlRoot->findChild<QObject*>("buttonNone");
        if (buttonNone) 
        {
            buttonNone->setProperty("checked", true);
        }
    }
    // coin win condition
    else if (type == 1)
    {
        QObject* buttonCoins = m_qmlRoot->findChild<QObject*>("buttonCoins");
        if (buttonCoins) 
        {
            buttonCoins->setProperty("checked", true);
        }        
        QObject* coins = m_qmlRoot->findChild<QObject*>("coinInput");
        if (coins) 
        {
            coins->setProperty("text", QString::number(value));
        }

    }
    // time win condition
    else if (type == 2)
    {
        QObject* buttonTime = m_qmlRoot->findChild<QObject*>("buttonTime");
        if (buttonTime) 
        {
            buttonTime->setProperty("checked", true);
        }
        QObject* time = m_qmlRoot->findChild<QObject*>("timeInput");
        if (time) 
        {
            time->setProperty("text", QString::number(value));
        }
    }
}


// Add rows to canvas
void LevelCanvas::addRowsAbove(int rows)
{
    if (rows <= 0)
        return;

    // Move old Tile to the bottom
    QMap<QPair<int, int>, int> newData;
    for (auto it = m_levelData.constBegin(); it != m_levelData.constEnd(); ++it)
    {
        newData.insert(
            qMakePair(it.key().first, it.key().second + rows),
            it.value());
    }

    // Add new Tiles above
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < m_gridWidth; ++x)
        {
            // left and right wall
            if (x == 0 || x == m_gridWidth - 1)
                newData[qMakePair(x, y)] = 84;
            // everything else stay same
        }
    }

    // Modify Grid
    m_levelData = newData;
    m_gridHeight += rows;
    setHeight(m_gridHeight * m_tileHeight);
    emit gridHeightChanged();

    update();

    qDebug() << "[LevelCanvas] Added" << rows << "rows ABOVE. New height:" << m_gridHeight;
}