#include "LevelCanvas.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QImage>
#include <QFile>
#include <QDataStream>

LevelCanvas::LevelCanvas(QQuickItem* parent)
    : QQuickPaintedItem(parent)
    , m_backgroundColor(92, 130, 161)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
}

void LevelCanvas::setTileset(const QString& path, int tileW, int tileH, int offset)
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

            if (row == 0 && col < 4) {
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

void LevelCanvas::paint(QPainter* painter)
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
                                m_tileWidth,  // Only 34px wide
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

void LevelCanvas::mousePressEvent(QMouseEvent* event)
{
    int gridX = event->x() / m_tileWidth;
    int gridY = event->y() / m_tileHeight;

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

void LevelCanvas::saveLevel(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "Cannot save level:" << path;
        return;
    }

    QDataStream out(&file);
    out << m_gridWidth << m_gridHeight;
    out << m_levelData.size();

    for (auto it = m_levelData.constBegin(); it != m_levelData.constEnd(); ++it)
    {
        out << it.key().first << it.key().second << it.value();
    }

    file.close();
    qDebug() << "Level saved:" << path;
}

void LevelCanvas::loadLevel(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Cannot load level:" << path;
        return;
    }

    QDataStream in(&file);
    in >> m_gridWidth >> m_gridHeight;

    int count;
    in >> count;

    m_levelData.clear();
    for (int i = 0; i < count; i++)
    {
        int x, y, tileIndex;
        in >> x >> y >> tileIndex;
        m_levelData[QPair<int, int>(x, y)] = tileIndex;
    }

    file.close();
    update();
    qDebug() << "Level loaded:" << path;
}
