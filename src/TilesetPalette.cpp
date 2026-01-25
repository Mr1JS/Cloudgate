#include "include/TilesetPalette.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QImage>

TilesetPalette::TilesetPalette(QQuickItem* parent)
    : QQuickPaintedItem(parent)
    , m_backgroundColor(92, 130, 161)
{
    setAcceptedMouseButtons(Qt::LeftButton);
}

<<<<<<< HEAD
void TilesetPalette::setTileset(const QList<Tile>& tiles, int tileW, int tileH, int offset, int endIndex)
{
    m_tiles      = tiles;
    m_tileWidth  = tileW;
    m_tileHeight = tileH;
    m_tileOffset = offset;
    m_endIndex   = endIndex;

    emit tileCountChanged();
    update();
=======
void TilesetPalette::loadTileset(const QString& path, int tileW, int tileH, int offset, int endIndex)
{
    QPixmap pix(path);
    if (pix.isNull())
    {
        qWarning() << "Tileset not found:" << path;
        return;
    }

    m_tileWidth = tileW;
    m_tileHeight = tileH;
    m_tileOffset = offset;
    m_endIndex = endIndex;

    extractTilesWithTransparency(pix, offset);

    update();
    qDebug() << "Tiles loaded:" << m_tiles.size();
}

void TilesetPalette::extractTilesWithTransparency(const QPixmap& pixmap, int offset)
{
    m_tiles.clear();

    QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);

    // Replace background color with transparency
    int tolerance = 10;

    for (int y = 0; y < img.height(); y++)
    {
        for (int x = 0; x < img.width(); x++)
        {
            QRgb pixel = img.pixel(x, y);
            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);

            // Check if color is close to background color
            if (qAbs(r - 92) <= tolerance &&
                qAbs(g - 130) <= tolerance &&
                qAbs(b - 161) <= tolerance)
            {
                img.setPixel(x, y, qRgba(0, 0, 0, 0)); // Transparent
            }
        }
    }

    QPixmap processedPixmap = QPixmap::fromImage(img);

    // Calculate with offset: each tile has offset pixels around it
    int totalTileWidth = m_tileWidth + offset;
    int totalTileHeight = m_tileHeight + offset;

    int cols = (pixmap.width() + offset) / totalTileWidth;
    int rows = (pixmap.height() + offset) / totalTileHeight;

    qDebug() << "Tileset size:" << pixmap.width() << "x" << pixmap.height();
    qDebug() << "With offset" << offset << "-> Calculated:" << cols << "columns x" << rows << "rows";
    qDebug() << "Tile size (with spacing):" << totalTileWidth << "x" << totalTileHeight;

    int idx = 0;
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            Tile t;
            t.index = idx++;
            t.pixmap = processedPixmap;

            // IMPORTANT: Exact calculation
            int x = offset + (col * totalTileWidth);
            int y = offset + (row * totalTileHeight);

            // IMPORTANT: Cut out FULL tile size incl. spacing      // IMPORTANT: CHANGED TO OT CUT FULL
            t.sourceRect = QRect(x, y, totalTileWidth-offset, totalTileHeight-offset);

            if (idx <= 3) {
                qDebug() << "Palette Tile" << idx-1 << "sourceRect:" << t.sourceRect
                         << "(full size with spacing)";
            }

            m_tiles.append(t);
        }
    }

    qDebug() << "Tiles extracted:" << m_tiles.size();
    emit tileCountChanged();
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557
}

void TilesetPalette::paint(QPainter* painter)
{
    if (m_tiles.isEmpty())
    {
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, false);

    // Calculate columns based on available width
    int columns = qMax(1, (int)(width() / (m_tileWidth + m_spacing)));
    int start = 0;
    int end = m_endIndex;
    // choose either default Tiles or 1x2 Tiles
    if (m_extraTiles)
    {
        start = m_endIndex;
        end = m_tiles.size();
    }

    for (int i = start; i < end; i++)
    {
        int row = (i - start) / columns;
        int col = (i - start) % columns;

        QRect targetRect(m_spacing + col * (m_tileWidth + m_spacing),
                         m_spacing + row * (m_tileHeight + m_spacing),
                         m_tileWidth,
                         m_tileHeight);

        // Draw tile exactly with sourceRect
        painter->drawPixmap(targetRect, m_tiles[i].pixmap, m_tiles[i].sourceRect);

        // Highlight on selection
        if (m_tiles[i].index == m_selectedIndex)
        {
            painter->setPen(QPen(QColor(0, 255, 0), 3));
            painter->drawRect(targetRect.adjusted(1, 1, -1, -1));
        }
    }
}
// in 1x2 Tiles / extraTiles Mode
void TilesetPalette::setExtraTiles(bool mode)
{
    m_extraTiles = mode;
    update();
}
void TilesetPalette::mousePressEvent(QMouseEvent* event)
{
    int columns = qMax(1, (int)(width() / (m_tileWidth + m_spacing)));

    // Improved click detection
<<<<<<< HEAD
    const QPointF pos = event->position();
    int clickX = int(pos.x());
    int clickY = int(pos.y());
=======
    int clickX = event->x();
    int clickY = event->y();
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557

    // Calculate which grid cell was clicked
    int cellX = clickX / (m_tileWidth + m_spacing);
    int cellY = clickY / (m_tileHeight + m_spacing);

    // Check if inside the tile (not in spacing)
    int xInCell = clickX - (cellX * (m_tileWidth + m_spacing));
    int yInCell = clickY - (cellY * (m_tileHeight + m_spacing));

    // Only if clicked inside tile (not spacing)
    if (xInCell >= m_spacing && xInCell < (m_spacing + m_tileWidth) &&
        yInCell >= m_spacing && yInCell < (m_spacing + m_tileHeight))
    {
        int idx = cellY * columns + cellX;
        // get real Index in extraTiles mode
        if (m_extraTiles)
        {
            idx += m_endIndex;
        }
        if (idx >= 0 && idx < m_tiles.size())
        {
            m_selectedIndex = idx;
            update();
            emit tileSelected(idx);
            qDebug() << "Tile selected:" << idx;
        }
    }
}
