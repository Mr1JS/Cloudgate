#include "LevelEditor.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

LevelEditor::LevelEditor(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    setAcceptedMouseButtons(Qt::LeftButton);
}

void LevelEditor::loadTileset(const QString& path, int tileW, int tileH)
{
    QPixmap pix(path);
    if (pix.isNull())
    {
        qWarning() << "Tileset not found:" << path;
        return;
    }

    m_tiles.clear();
    m_tileWidth = tileW;
    m_tileHeight = tileH;
    // tileset calculation based on img width and height, maybe change because its static for our purpose?
    int cols = pix.width() / (tileW + m_tileOffset);
    int rows = pix.height() / (tileH + m_tileOffset);
    int idx = 0;
    // get pixel coordinates for each tile
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            Tile t;
            t.index = idx++;
            t.pixmap = pix;
            t.sourceRect = QRect(m_tileOffset + (x+1) * (tileW + m_tileOffset),
                                 m_tileOffset + (y+1) * (tileH + m_tileOffset),
                                 tileW,
                                 tileH);
            m_tiles.append(t);
            qDebug() << x+1 << " " << y+;
        }
    }

    update();
    qDebug() << "Tiles loaded:" << m_tiles.size();
}

void LevelEditor::paint(QPainter* painter)
{
    if (m_tiles.isEmpty())
    {
        return;
    }
    // spacing between  elements
    int spacing = 6;
    // same problem, maybe add fixed columns  and row sizes
    int columns = width() / (m_tileWidth + spacing);
    if (columns < 1) columns = 1;

    int c = 0;
    // draw tiles
    for (auto& tile : m_tiles)
    {
        int row = c / columns;
        int col = c % columns;

        QRect targetRect(spacing + col * (m_tileWidth + spacing),
                         spacing + row * (m_tileHeight + spacing),
                         m_tileWidth,
                         m_tileHeight);

        painter->drawPixmap(targetRect, tile.pixmap, tile.sourceRect);
        // selection design choice: green border
        if (tile.index == m_selectedIndex)
        {
            painter->setPen(QPen(Qt::green, 2));
            painter->drawRect(targetRect.adjusted(0, 0, -1, -1));
        }
        // c++
        c++;
    }
}

void LevelEditor::mousePressEvent(QMouseEvent* event)
{
    // attention: match void paint here, just for testing?
    int spacing = 6;
    int columns = width() / (m_tileWidth + spacing);
    if (columns < 1)
    {
        columns = 1;

    }
    // check where clicked
    int col = event->x() / (m_tileWidth + spacing);
    int row = event->y() / (m_tileHeight + spacing);
    int id = row * columns + col;
    // if element clicked -> selected
    if (id >= 0 && id < m_tiles.size())
    {
        m_selectedIndex = id;
        update();
        emit tileClicked(id);
        qDebug() << "Tile clcked:" << id;
    }
}
