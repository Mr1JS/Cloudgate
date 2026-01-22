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

static std::vector<int> collisionMapToFlat(
    const QMap<QPair<int, int>, int> &collision,
    int width, int height)
{
    std::vector<int> out(width * height, 0);
    for (auto it = collision.begin(); it != collision.end(); ++it)
    {
        const int x = it.key().first;
        const int y = it.key().second;
        if (x >= 0 && x < width && y >= 0 && y < height)
            out[y * width + x] = it.value();
    }
    return out;
}

static QMap<QPair<int, int>, int> flatToCollisionMap(
    const std::vector<int> &flat,
    int width, int height)
{
    QMap<QPair<int, int>, int> out;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int v = flat[y * width + x];
            if (v != 0)
                out.insert(qMakePair(x, y), v);
        }
    }
    return out;
}

LevelCanvas::LevelCanvas(QQuickItem *parent)
    : QQuickPaintedItem(parent), m_backgroundColor(92, 130, 161)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
}

void LevelCanvas::setTileset(const QString &path, int tileW, int tileH, int offset)
{
    QPixmap pix(path);
    if (pix.isNull())
    {
        qWarning() << "Tileset not found:" << path;
        return;
    }

    m_tileWidth = tileW;
    m_tileHeight = tileH;

    // Load tiles with transparency - EXACTLY like palette
    m_tiles.clear();
    QImage img = pix.toImage().convertToFormat(QImage::Format_ARGB32);

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

        if (event->button() == Qt::RightButton)
        {
            // Right click: Delete tile
            if (m_levelData.contains(pos))
            {
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

            m_levelData[pos] = m_currentTileIndex;
            update();
            qDebug() << "Tile placed at (" << gridX << "," << gridY << ") with index" << m_currentTileIndex;
        }
    }
}

void LevelCanvas::saveLevel(const QString &xmlPath)
{
    QString p = xmlPath;

    // Expand "~" to home directory
    if (p.startsWith("~/"))
        p.replace(0, 1, QDir::homePath());
        
    QFileInfo xmlInfo(p);
    const QString dir = xmlInfo.absolutePath();
    const QString h5FileName = "level_trial.h5";
    const QString h5Path = dir + "/" + h5FileName;

    const int w = m_gridWidth;
    const int h = m_gridHeight;

    // Always produce a full grid (all zeros if empty)
    std::vector<int> flat = collisionMapToFlat(m_collisionData, w, h);

    // --- Save HDF5 collision ---
    try
    {
        LevelHdf5IO io;
        io.open(h5Path.toStdString());

        std::vector<size_t> dim = {(size_t)h, (size_t)w};
        // TODO: adjust chunk size? in this case small level so fixed 32x32 (minimal)
        std::vector<hsize_t> chunkSize = {
            std::min<hsize_t>(32, (hsize_t)h),
            std::min<hsize_t>(32, (hsize_t)w)};

        // BaseHdf5IO expects shared_array<T>
        jumper::shared_array<int> arr(new int[flat.size()]);
        std::copy(flat.begin(), flat.end(), arr.get());

        io.save<int>("tiles", "collision_tiles", dim, chunkSize, arr);
        qDebug() << "[saveLevel] HDF5 collision saved:" << h5Path;
    }
    catch (const std::exception &e)
    {
        qWarning() << "[saveLevel] HDF5 collision save failed:" << e.what();
        return;
    }

    // --- Save XML (always) ---
    QFile file(p);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qWarning() << "[saveLevel] Cannot write XML:" << xmlPath;
        return;
    }
    qDebug() << "[saveLevel] Writing XML:" << xmlPath;

    QTextStream ts(&file);
    ts << "<?xml version=\"1.0\"?>\n";
    ts << "<level resources=\"" << h5FileName << "\">\n";
    ts << "  <collision_tiles tileset=\"default\" src=\"collision_tiles\"/>\n";
    ts << "</level>\n";
    file.close();
}

void LevelCanvas::loadLevel(const QString &xmlPath)
{
    QString p = xmlPath;

    // Expand "~" to home directory
    if (p.startsWith("~/"))
        p.replace(0, 1, QDir::homePath());

    QFileInfo xmlInfo(p);
    const QString dir = xmlInfo.absolutePath();

    QString h5FileName = "level.h5";
    QString collisionDataset = "collision_tiles";

    // --- Load XML to get resources name (and optionally src=...) ---
    {
        QFile file(xmlInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly))
        {
            qWarning() << "[loadLevel] Cannot read XML:" << xmlPath;
            m_collisionData.clear();
            return;
        }
        qDebug() << "[loadLevel] Reading XML:" << xmlPath;
        const QString xmlText = QString::fromUtf8(file.readAll());
        file.close();

        // resources="..."
        QRegularExpression reRes("resources\\s*=\\s*\"([^\"]+)\"");
        auto m = reRes.match(xmlText);
        if (m.hasMatch())
            h5FileName = m.captured(1);

        // src="..."
        QRegularExpression reSrc("<collision_tiles[^>]*tiles\\s*=\\s*\"([^\"]+)\"");
        auto m2 = reSrc.match(xmlText);
        if (m2.hasMatch())
            collisionDataset = m2.captured(1);
    }

    const QString h5Path = dir + "/" + h5FileName;

    const int w = m_gridWidth;
    const int h = m_gridHeight;

    qDebug() << "[loadLevel] Loading level from HDF5:" << h5Path;
    // --- Load collision from HDF5 (if missing => empty) ---
    try
    {
        LevelHdf5IO io;
        io.open(h5Path.toStdString());

        qDebug() << "[loadLevel] Loading collision dataset:" << collisionDataset;
        std::vector<size_t> dim = {(size_t)h, (size_t)w};

        auto shared = io.loadArray<int>("tiles", collisionDataset.toStdString(), dim);

        // Copy into vector<int>
        std::vector<int> flat((size_t)w * (size_t)h, 0);
        std::copy(shared.get(), shared.get() + flat.size(), flat.begin());

        m_levelData = flatToCollisionMap(flat, w, h);
        qDebug() << "[loadLevel] collision loaded with" << m_levelData.size() << "tiles";
    }
    catch (const std::exception &e)
    {
        // If file/dataset not found => empty collision level
        qWarning() << "[loadLevel] collision missing => empty:" << e.what();
        m_levelData.clear();
    }

    update();
}
