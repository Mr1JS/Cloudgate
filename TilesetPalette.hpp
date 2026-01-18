#ifndef TILESETPALETTE_HPP
#define TILESETPALETTE_HPP

#include <QQuickPaintedItem>
#include <QPixmap>
#include <QList>
#include <QRect>
#include <QColor>

// Tile structure for individual tiles
struct Tile {
    int index;
    QPixmap pixmap;
    QRect sourceRect;
};

// Tileset palette for tile selection
class TilesetPalette : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int tileCount READ tileCount NOTIFY tileCountChanged)

public:
    explicit TilesetPalette(QQuickItem* parent = nullptr);

    Q_INVOKABLE void loadTileset(const QString& path, int tileW = 34, int tileH = 34, int offset = 0);

    int tileCount() const { return m_tiles.size(); }

    void paint(QPainter* painter) override;

signals:
    void tileSelected(int index);
    void tileCountChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    void extractTilesWithTransparency(const QPixmap& pixmap, int offset);

    QList<Tile> m_tiles;
    int m_tileWidth = 34;
    int m_tileHeight = 34;
    int m_tileOffset = 0;
    int m_selectedIndex = -1;
    int m_spacing = 6;
    QColor m_backgroundColor; // RGB(92, 130, 161) treated as transparent
};

#endif // TILESETPALETTE_HPP
