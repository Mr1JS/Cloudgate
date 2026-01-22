#ifndef LEVELCANVAS_HPP
#define LEVELCANVAS_HPP

#include <QQuickPaintedItem>
#include <QPixmap>
#include <QList>
#include <QMap>
#include <QRect>
#include <QColor>
#include "TilesetPalette.hpp"

// Level canvas for placing tiles
class LevelCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int gridWidth READ gridWidth WRITE setGridWidth NOTIFY gridWidthChanged)
    Q_PROPERTY(int gridHeight READ gridHeight WRITE setGridHeight NOTIFY gridHeightChanged)

public:
    explicit LevelCanvas(QQuickItem* parent = nullptr);

    Q_INVOKABLE void setTileset(const QString& path, int tileW = 32, int tileH = 32, int offset = 0, int endIndex = 20);
    Q_INVOKABLE void placeTile(int tileIndex);
    Q_INVOKABLE void clearLevel();
    Q_INVOKABLE void saveLevel(const QString& path);
    Q_INVOKABLE void loadLevel(const QString& path);

    int gridWidth() const { return m_gridWidth; }
    void setGridWidth(int w) { m_gridWidth = w; emit gridWidthChanged(); update(); }

    int gridHeight() const { return m_gridHeight; }
    void setGridHeight(int h) { m_gridHeight = h; emit gridHeightChanged(); update(); }
    void setExtraTiles(bool mode);
    void paint(QPainter* painter) override;

signals:
    void gridWidthChanged();
    void gridHeightChanged();

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    QList<Tile> m_tiles;
    QMap<QPair<int, int>, int> m_levelData; // Grid position -> Tile index

    int m_tileWidth = 32;
    int m_tileHeight = 32;
    int m_gridWidth = 20;
    int m_gridHeight = 25;
    int m_currentTileIndex = -1;
    QColor m_backgroundColor;
    bool m_extraTiles = false;
    int m_endIndex = 20; // at which point do the tiles split up between normal Tiles and extraTiles
};

#endif // LEVELCANVAS_HPP
