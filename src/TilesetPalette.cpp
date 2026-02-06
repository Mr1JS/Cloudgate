#include "include/TilesetPalette.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QImage>
#include <QFile>
#include <QXmlStreamReader>
#include <QDir>
#include <QCoreApplication>

TilesetPalette::TilesetPalette(QQuickItem *parent)
    : QQuickPaintedItem(parent), m_backgroundColor(92, 130, 161)
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


void TilesetPalette::loadTileNames(const QString &xmlPath)
{
    m_tileNames.clear();

    // Try to find the XML file
    QString absolutePath = xmlPath;
    if (!QFile::exists(absolutePath))
    {
        // Try relative to application directory
        QDir appDir(QCoreApplication::applicationDirPath());
        appDir.cdUp();
        absolutePath = appDir.absoluteFilePath(xmlPath);

        if (!QFile::exists(absolutePath))
        {
            // Try from current directory
            absolutePath = QDir::current().absoluteFilePath(xmlPath);
        }
    }

    QFile file(absolutePath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        // names not needed currently?
        //qWarning() << "Could not open RulesTiles.xml:" << absolutePath;
        return;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd())
    {
        xml.readNext();

        if (xml.isStartElement() && xml.name() == QString("tile"))
        {
            QXmlStreamAttributes attributes = xml.attributes();

            if (attributes.hasAttribute("id") && attributes.hasAttribute("name"))
            {
                int id = attributes.value("id").toInt();
                QString name = attributes.value("name").toString();
                m_tileNames[id] = name;
            }
        }
    }

    if (xml.hasError())
    {
        qWarning() << "XML parsing error:" << xml.errorString();
    }

    file.close();
    qDebug() << "Loaded" << m_tileNames.size() << "tile names from" << absolutePath;
}

void TilesetPalette::extractTilesWithTransparency(const QPixmap &pixmap, int offset)
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
            t.sourceRect = QRect(x, y, totalTileWidth - offset, totalTileHeight - offset);

            if (idx <= 3)
            {
                qDebug() << "Palette Tile" << idx - 1 << "sourceRect:" << t.sourceRect
                         << "(full size with spacing)";
            }

            m_tiles.append(t);
        }
    }

    qDebug() << "Tiles extracted:" << m_tiles.size();
    emit tileCountChanged();
    update();
}

void TilesetPalette::paint(QPainter *painter)
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
        // loop in steps of two so it draws every second tile below the first
        for (int i = start; i < end - 1; i += 2)
        {
            int row = (i - start) / (columns * 2) * 2;
            int col = ((i - start) / 2) % columns;
            // upper part of extraTile
            QRect targetRect(m_spacing + col * (m_tileWidth + m_spacing),
                             m_spacing + row * (m_tileHeight + m_spacing / 2),
                             m_tileWidth,
                             m_tileHeight);
            // draw upper part
            painter->drawPixmap(targetRect, m_tiles[i].pixmap, m_tiles[i].sourceRect);

            // lower part of extraTile
            QRect targetRect2(m_spacing + col * (m_tileWidth + m_spacing),
                              (row + 1) * (m_tileHeight + m_spacing / 2),
                              m_tileWidth,
                              m_tileHeight);
            // draw lower part
            painter->drawPixmap(targetRect2, m_tiles[i + 1].pixmap, m_tiles[i + 1].sourceRect);

            // Highlight both together on selection
            if (m_tiles[i + 1].index == m_selectedIndex)
            {
                painter->setPen(QPen(QColor(0, 255, 0), 3));
                painter->drawRect(targetRect2.adjusted(1, -31, -1, -1));
            }
        }
    }
    else
    { // only one tile / normal tiles
        for (int i = start; i < end; i++)
        {
            int row = (i - start) / columns;
            int col = (i - start) % columns;

            // size of tile
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
}
// in 1x2 Tiles / extraTiles Mode
void TilesetPalette::setExtraTiles(bool mode)
{
    m_extraTiles = mode;
    update();
}
void TilesetPalette::mousePressEvent(QMouseEvent *event)
{
    int columns = qMax(1, (int)(width() / (m_tileWidth + m_spacing)));

    // get position of click
    const QPointF pos = event->position();
    int clickX = int(pos.x());
    int clickY = int(pos.y());

    // calculate height based on current Tileset mode
    int heightMultiplier = (m_extraTiles) ? 2 : 1;

    // Calculate which grid cell was clicked
    int cellX = clickX / (m_tileWidth + m_spacing);
    int cellY = clickY / (m_tileHeight * heightMultiplier + m_spacing);

    // Check if inside the tile (not in spacing)
    int xInCell = clickX - (cellX * (m_tileWidth + m_spacing));
    int yInCell = clickY - (cellY * (m_tileHeight * heightMultiplier + m_spacing));

    // Only if clicked inside tile (not spacing)
    if (xInCell >= m_spacing && xInCell < (m_spacing + m_tileWidth) &&
        yInCell >= m_spacing && yInCell < (m_spacing + m_tileHeight * heightMultiplier))
    {
        // id of visible element on page
        int idx = cellY * columns + cellX;

        // if extraTiles, add the highest index of normal tiles, then 2x because its 2 tiles per clickable Element and +1 to always choose the lower part
        if (m_extraTiles)
        {
            idx += m_endIndex + idx + 1;
        }

        if (idx >= 0 && idx < m_tiles.size())
        {
            // save selected id
            m_selectedIndex = idx;
            update();
            emit tileSelected(idx);

            // Show ID and name from XML
            QString tileName = m_tileNames.value(idx, "Unknown"); // XML starts at ID=0 too
            qDebug() << "Tile selected - ID:" << (idx) << "Name:" << tileName;
        }
    }
}
