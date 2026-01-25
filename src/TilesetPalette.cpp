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

void TilesetPalette::setTileset(const QList<Tile>& tiles, int tileW, int tileH, int offset, int endIndex)
{
    m_tiles      = tiles;
    m_tileWidth  = tileW;
    m_tileHeight = tileH;
    m_tileOffset = offset;
    m_endIndex   = endIndex;

    emit tileCountChanged();
    update();
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
    const QPointF pos = event->position();
    int clickX = int(pos.x());
    int clickY = int(pos.y());

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
