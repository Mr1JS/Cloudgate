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
#include <QRegularExpression>
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

void LevelCanvas::setTileset(const QString &path, int tileW, int tileH, int offset, int endIndex)
{
    qDebug() << "[LevelCanvas] setTileset:" << path
             << "tileW=" << tileW
             << "tileH=" << tileH
             << "offset=" << offset;

    // remember meta for saving
    m_tilesetPath = path;
    m_tilesetTextureName = QFileInfo(path).baseName(); // "tileset1"

    QPixmap pix(path);
    if (pix.isNull())
    {
        qWarning() << "Tileset not found:" << path;
        return;
    }

    m_tileWidth = tileW;
    m_tileHeight = tileH;
    m_endIndex = endIndex;
    m_tileOffset = offset;

    // load tileset image (used for palette AND for saving into H5)
    QImage img(path);
    if (img.isNull())
    {
        qWarning() << "[LevelCanvas] Could not load tileset image:" << path;
        m_tilesetImage = QImage();
        return;
    }

    // enforce RGBA (so H5 export is stable)
    m_tilesetImage = img.convertToFormat(QImage::Format_RGBA8888);

    // Load tiles with transparency - EXACTLY like palette
    m_tiles.clear();
    img = pix.toImage().convertToFormat(QImage::Format_ARGB32);

    int tolerance = 10;

    for (int y = 0; y < img.height(); y++)
    {
        for (int x = 0; x < img.width(); x++)
        {
            QRgb pixel = img.pixel(x, y);
            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);

            if (qAbs(r - 92) <= tolerance &&
                qAbs(g - 130) <= tolerance &&
                qAbs(b - 161) <= tolerance)
            {
                img.setPixel(x, y, qRgba(0, 0, 0, 0));
            }
        }
    }

    QPixmap processedPixmap = QPixmap::fromImage(img);

    // EXACTLY same calculation as palette
    int totalTileWidth = m_tileWidth + offset;
    int totalTileHeight = m_tileHeight + offset;

    int cols = (pix.width() + offset) / totalTileWidth;
    int rows = (pix.height() + offset) / totalTileHeight;

    qDebug() << "=== CANVAS TILESET LOADING ===";
    qDebug() << "Tile size (usable):" << m_tileWidth << "x" << m_tileHeight;
    qDebug() << "Tile size (with spacing):" << totalTileWidth << "x" << totalTileHeight;

    int idx = 0;
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            Tile t;
            t.index = idx++;
            t.pixmap = processedPixmap;

            int x = offset + (col * totalTileWidth);
            int y = offset + (row * totalTileHeight);

            // IMPORTANT: Cut out FULL tile size (incl. spacing)
            // But only inner pixels are visible
            t.sourceRect = QRect(x, y, totalTileWidth, totalTileHeight);

            if (row == 0 && col < 4)
            {
                qDebug() << "Canvas Tile" << t.index
                         << "-> sourceRect:" << t.sourceRect
                         << "(contains" << m_tileWidth << "x" << m_tileHeight << "+ spacing)";
            }

            m_tiles.append(t);
        }
    }

    update();
    qDebug() << "Canvas tileset loaded:" << m_tiles.size() << "tiles";
    qDebug() << "==============================";
}

void LevelCanvas::placeTile(int tileIndex)
{
    m_currentTileIndex = tileIndex;
    qDebug() << "Current tile set:" << tileIndex;
}

void LevelCanvas::clearLevel()
{
    m_levelData.clear();
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
                        QPair<int, int> pos2(gridX, gridY-1);
                        m_levelData.remove(pos2);
                        qDebug() << "Tile deleted at (" << gridX << "," << gridY-1 << ")";
                    }
                    else
                    {
                        QPair<int, int> pos2(gridX, gridY+1);
                        m_levelData.remove(pos2);
                        qDebug() << "Tile deleted at (" << gridX << "," << gridY+1 << ")";
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
                    QPair<int, int> pos2(gridX, gridY-1);
                    // special case: extraTile on extraTile
                    if (m_levelData.contains(pos2))
                    {
                        if (m_levelData[pos2] >= m_endIndex && m_levelData[pos2] % 2 != m_endIndex % 2)
                        {
                            QPair<int, int> pos3(gridX, gridY-2);
                            m_levelData.remove(pos3);
                        }
                    }
                    m_levelData[pos2] = m_currentTileIndex-1;

                }
                // check if Upper or Lower and not outside of grid
                else if (m_currentTileIndex % 2 == m_endIndex % 2 && gridY != m_gridHeight-1)
                {
                    QPair<int, int> pos2(gridX, gridY+1);
                    // special case: extraTile on extraTile
                    if (m_levelData.contains(pos2))
                    {
                        if (m_levelData[pos2] >= m_endIndex && m_levelData[pos2] % 2 == m_endIndex % 2)
                        {
                            QPair<int, int> pos3(gridX, gridY+2);
                            m_levelData.remove(pos3);
                        }
                    }
                    m_levelData[pos2] = m_currentTileIndex+1;
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
        out[y * w + x] = it.value();
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
            const int v = flat[y * w + x];
            if (v != defaultValue)
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
        return false;

    QImage img = image.convertToFormat(QImage::Format_RGBA8888);

    H = img.height();
    W = img.width();

    outBytes.resize((size_t)H * (size_t)W * 4);
    memcpy(outBytes.data(), img.bits(), outBytes.size());
    return true;
}

// -----------------------------------------------
// Helper: convert RGBA bytes back to QImage
// -----------------------------------------------
static QImage rgbaBytesToImage(const std::vector<unsigned char> &bytes, int H, int W)
{
    if ((int)bytes.size() != H * W * 4)
        return QImage();

    QImage img(W, H, QImage::Format_RGBA8888);
    memcpy(img.bits(), bytes.data(), bytes.size());
    return img;
}

// -----------------------------------------------
// Small XML helper: extract attribute value from a single tag line
// e.g. <collision_tiles texture="tileset1" tiles="level1">
// -----------------------------------------------
static QString extractAttr(const QString &line, const QString &attrName)
{
    QRegularExpression re(attrName + R"delim(\s*=\s*"([^"]*)")delim");
    auto m = re.match(line);
    if (!m.hasMatch())
        return "";
    return m.captured(1);
}

// ============================================================================
// SAVE LEVEL (XML + H5)
// background_tiles = m_levelData -> /tiles/level2
// collision_tiles  = m_collisionData -> /tiles/level1
// tileset texture  = /textures/<tilesetTextureName>
// ============================================================================
void LevelCanvas::saveLevel(const QString &xmlPath)
{
    qDebug() << "[LevelCanvas] saveLevel ->" << xmlPath;

    // expand ~
    QString p = xmlPath;
    if (p.startsWith("~"))
        p.replace(0, 1, QDir::homePath());

    QFileInfo xmlInfo(p);
    const QString outDir   = xmlInfo.absolutePath();
    const QString baseName = xmlInfo.completeBaseName(); // level_trial
    const QString h5Name   = baseName + ".h5";
    const QString h5Path   = outDir + "/" + h5Name;

    // Your reference h5 has {32,73}
    const int gridH = 32;
    const int gridW = 73;

    // collision_tiles  = m_collisionData -> /tiles/level1
    std::vector<int> flatCollision = flattenTileMap(m_levelData, gridW, gridH, 0);

    std::vector<size_t> tileDim = { (size_t)gridH, (size_t)gridW };

    // save tileset into /textures/<tilesetTextureName>
    if (m_tilesetTextureName.isEmpty() && !m_tilesetPath.isEmpty())
        m_tilesetTextureName = QFileInfo(m_tilesetPath).baseName();

    if (m_tilesetImage.isNull() && !m_tilesetPath.isEmpty())
    {
        QImage loaded(m_tilesetPath);
        if (!loaded.isNull())
            m_tilesetImage = loaded.convertToFormat(QImage::Format_RGBA8888);
    }

    // ---- NEW: Load background image from Qt resources and store into H5 like tileset ----
    const QString bgResourcePath = ":/resources/images/clouds2.png";
    const QString bgTextureName  = "clouds2";  // dataset name under /textures

    QImage bgImg(bgResourcePath);
    if (bgImg.isNull())
    {
        qWarning() << "[LevelCanvas] Background PNG not found in qrc:" << bgResourcePath;
        // we continue saving the level anyway (tiles + tileset)
    }
    else
    {
        bgImg = bgImg.convertToFormat(QImage::Format_RGBA8888);
    }

    // ---- NEW: Load actor image from Qt resources and store into H5 like tileset ----
    const QString actorResourcePath = ":/resources/images/mario1.png";
    const QString actorTextureName  = "mario1";  // dataset name under /textures
    QImage actorImg(actorResourcePath);
    if (actorImg.isNull())
    {
        qWarning() << "[LevelCanvas] Actor PNG not found in qrc:" << actorResourcePath;
        // we continue saving the level anyway (tiles + tileset)
    }
    else
    {
        actorImg = actorImg.convertToFormat(QImage::Format_RGBA8888);
    }

    // ---- H5 SAVE using CRTP BaseHdf5IO ----
    try
    {
        using IO = jumper::BaseHdf5IO<
            jumper::hdf5features::TextureIO,
            jumper::hdf5features::TileSetIO>;

        IO io;
        io.open(h5Path.toStdString());

        // ---- tileset texture ----
        if (!m_tilesetImage.isNull() && !m_tilesetTextureName.isEmpty())
        {
            std::vector<unsigned char> texBytes;
            int texH = 0, texW = 0;

            if (imageToRgbaBytes(m_tilesetImage, texBytes, texH, texW))
            {
                std::vector<size_t> texDim = { (size_t)texH, (size_t)texW, (size_t)4 };
                auto texArr = makeSharedArrayCopy(texBytes);

                io.save<unsigned char>("textures",
                                       m_tilesetTextureName.toStdString(),
                                       texDim,
                                       texArr);
            }
        }

        // ---- NEW: background png saved as texture ----
        if (!bgImg.isNull())
        {
            std::vector<unsigned char> bgBytes;
            int bgH = 0, bgW = 0;

            if (imageToRgbaBytes(bgImg, bgBytes, bgH, bgW))
            {
                std::vector<size_t> bgDim = { (size_t)bgH, (size_t)bgW, (size_t)4 };
                auto bgTexArr = makeSharedArrayCopy(bgBytes);

                io.save<unsigned char>("textures",
                                       bgTextureName.toStdString(),
                                       bgDim,
                                       bgTexArr);

                qDebug() << "[LevelCanvas] Saved background texture to H5: /textures/" << bgTextureName
                         << "dim=" << bgH << "x" << bgW << "x4";
            }
        }

        // ---- NEW: Save actor image as texture ----
        if (!actorImg.isNull())
        {
            std::vector<unsigned char> actorBytes;
            int actorH = 0, actorW = 0;

            if (imageToRgbaBytes(actorImg, actorBytes, actorH, actorW))
            {
                std::vector<size_t> actorDim = { (size_t)actorH, (size_t)actorW, (size_t)4 };
                auto actorTexArr = makeSharedArrayCopy(actorBytes);

                io.save<unsigned char>("textures",
                                       actorTextureName.toStdString(),
                                       actorDim,
                                       actorTexArr);

                qDebug() << "[LevelCanvas] Saved actor texture to H5: /textures/" << actorTextureName
                         << "dim=" << actorH << "x" << actorW << "x4";
            }
        }

        // ---- tiles ----
        auto colArr = makeSharedArrayCopy(flatCollision);
        io.save<int>("tiles", "level1", tileDim, colArr); // collision
    }
    catch (const std::exception &e)
    {
        qWarning() << "[LevelCanvas] H5 save failed:" << e.what();
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
        tilesPerRow = m_tilesetImage.width() / m_tileWidth;
        numRows     = m_tilesetImage.height() / m_tileHeight;
    }

    ts << "<level resources=\"" << h5Name << "\">\n";

    // background still references the tileset for tilemap usage
    // (If you want XML to reference the PNG instead, tell me and I adapt)
    ts << "  <background_tiles texture=\"" << bgTextureName << "\">\n";
    ts << "    <layer>0</layer>\n";
    ts << "  </background_tiles>\n";

    ts << "  <collision_tiles texture=\"" << m_tilesetTextureName << "\" tiles=\"level1\">\n";
    ts << "    <tileWidth>" << m_tileWidth << "</tileWidth>\n";
    ts << "    <tileHeight>" << m_tileHeight << "</tileHeight>\n";
    ts << "    <tilesPerRow>" << tilesPerRow << "</tilesPerRow>\n";
    ts << "    <numRows>" << numRows << "</numRows>\n";
    ts << "    <tileOffset>" << m_tileOffset << "</tileOffset>\n";
    ts << "    <layer>1</layer>\n";
    ts << "  </collision_tiles>\n";

    // Actor block (Dummy values for now) //TODO: if there are actor settings to save
    // then this should be adapted and changed accordingly
    ts << "  <actor texture=\"" << actorTextureName << "\">\n";
    ts << "    <num_frames>2</num_frames>\n";
    ts << "    <frame_width>18</frame_width>\n";
    ts << "    <frame_height>32</frame_height>\n";
    ts << "    <position_x>100</position_x>\n";
    ts << "    <position_y>600</position_y>\n";
    ts << "    <jump_force_y>-540.0</jump_force_y>\n";
    ts << "    <move_force_x>800.0</move_force_x>\n";
    ts << "    <max_run_velocity>122.0</max_run_velocity>\n";
    ts << "    <max_fall_velocity>250.0</max_fall_velocity>\n";
    ts << "    <max_jump_height>90</max_jump_height>\n";
    ts << "    <fps>12</fps>\n";
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

    qDebug() << "[LevelCanvas] Saved XML:" << p;
    qDebug() << "[LevelCanvas] Saved H5 :" << h5Path;
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

    QTextStream ts(&f);
    while (!ts.atEnd())
    {
        const QString line = ts.readLine().trimmed();

        if (line.startsWith("<level "))
        {
            h5FileName = extractAttr(line, "resources");
        }
        else if (line.startsWith("<background_tiles"))
        {
            bgTextureName = extractAttr(line, "texture");
        }
        else if (line.startsWith("<collision_tiles"))
        {
            colTextureName = extractAttr(line, "texture");
            colTilesDataset = extractAttr(line, "tiles");
        }
        else if (line.startsWith("<tileWidth>"))
        {
            bool ok = false;
            int v = line.mid(QString("<tileWidth>").size()).split("<").first().toInt(&ok);
            if (ok)
                m_tileWidth = v;
        }
        else if (line.startsWith("<tileHeight>"))
        {
            bool ok = false;
            int v = line.mid(QString("<tileHeight>").size()).split("<").first().toInt(&ok);
            if (ok)
                m_tileHeight = v;
        }
        else if (line.startsWith("<tileOffset>"))
        {
            bool ok = false;
            int v = line.mid(QString("<tileOffset>").size()).split("<").first().toInt(&ok);
            if (ok)
                m_tileOffset = v;
        }
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

    // -------- H5 LOAD via CRTP BaseHdf5IO --------
    try
    {
        using IO = jumper::BaseHdf5IO<
            jumper::hdf5features::TextureIO,
            jumper::hdf5features::TileSetIO>;

        IO io;
        io.open(h5Path.toStdString());

        // Load tiles
        // We need dims; if your loadArray requires dim param, use stored grid size.
        // Here we assume your loadArray returns vector + dim through argument.
        // If your BaseHdf5IO signature is different, tell me and I adjust.

        // -> collision tiles
        std::vector<size_t> colDim;
        auto colArr = io.loadArray<int>("tiles", colTilesDataset.toStdString(), colDim);

        size_t colN = 1;
        for (size_t d : colDim)
            colN *= d;

        std::vector<int> colFlat(colN);
        std::copy(colArr.get(), colArr.get() + colN, colFlat.begin());

        // Grid size from dataset dims
        int gridH = 32;
        int gridW = 73;
        if (colDim.size() >= 2)
        {
            gridH = (int)colDim[0];
            gridW = (int)colDim[1];
        }

        // IMPORTANT: these are grid dims, NOT tile pixel width/height!
        // So do NOT change m_tileWidth / m_tileHeight here.
        // (m_tileWidth/m_tileHeight come from xml <tileWidth>, <tileHeight>)

        m_gridHeight = gridH;
        m_gridWidth = gridW;

        // rebuild maps
        // m_collisionData = unflattenTileMap(colFlat, gridW, gridH, 0); // does not exist anymore, since m_levelData holds both
        m_levelData = unflattenTileMap(colFlat, gridW, gridH, 0);

        // ---- texture ----
        if (!m_tilesetTextureName.isEmpty())
        {
            std::vector<size_t> texDim;
            auto texArr = io.loadArray<unsigned char>("textures", m_tilesetTextureName.toStdString(), texDim);

            size_t texN = 1;
            for (size_t d : texDim)
                texN *= d;

            std::vector<unsigned char> texBytes(texN);
            std::copy(texArr.get(), texArr.get() + texN, texBytes.begin());

            if (texDim.size() == 3 && texDim[2] == 4)
            {
                int H = (int)texDim[0];
                int W = (int)texDim[1];
                m_tilesetImage = rgbaBytesToImage(texBytes, H, W);
            }
            else
            {
                qWarning() << "[LevelCanvas] Unexpected texture dims:" << (int)texDim.size();
            }
        }
    }
    catch (const std::exception &e)
    {
        qWarning() << "[LevelCanvas] H5 load failed:" << e.what();
        return;
    }
    
    update();

    qDebug() << "[LevelCanvas] Loaded level:"
             << "grid=" << m_tileWidth << "x" << m_tileHeight
             << "texture=" << m_tilesetTextureName;
}

// in 1x2 Tiles / extraTiles Mode
void LevelCanvas::setExtraTiles(bool mode)
{
    m_extraTiles = mode;
}