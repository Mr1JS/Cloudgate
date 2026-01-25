#ifndef TILESETPALETTE_HPP
#define TILESETPALETTE_HPP

#include <QQuickPaintedItem>
#include <QPixmap>
#include <QList>
#include <QRect>
#include <QColor>
#include "Tile.hpp"


// Tileset palette for tile selection
class TilesetPalette : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int tileCount READ tileCount NOTIFY tileCountChanged)

public:
    explicit TilesetPalette(QQuickItem* parent = nullptr);

<<<<<<< HEAD
    //Q_INVOKABLE void loadTileset(const QImage& path, int tileW = 32, int tileH = 32, int offset = 0, int endIndex = 20);
    Q_INVOKABLE void setTileset(const QList<Tile>& tiles, int tileW, int tileH, int offset, int endIndex);

=======
    Q_INVOKABLE void loadTileset(const QString& path, int tileW = 32, int tileH = 32, int offset = 0, int endIndex = 20);
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557

    int tileCount() const { return m_tiles.size(); }

    void setExtraTiles(bool mode);
    void paint(QPainter* painter) override;

signals:
    void tileSelected(int index);
    void tileCountChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
<<<<<<< HEAD
=======
    void extractTilesWithTransparency(const QPixmap& pixmap, int offset);
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557
    QList<Tile> m_tiles;
    int m_tileWidth = 32;
    int m_tileHeight = 32;
    int m_tileOffset = 0;
    int m_selectedIndex = -1;
    int m_spacing = 6;
    QColor m_backgroundColor; // RGB(92, 130, 161) treated as transparent
    bool m_extraTiles = false;
    int m_endIndex = 20; // at which point do the tiles split up between normal Tiles and extraTiles
};

#endif // TILESETPALETTE_HPP
