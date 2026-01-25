#ifndef LEVELCANVAS_HPP
#define LEVELCANVAS_HPP

#include <QQuickPaintedItem>
#include <QPixmap>
#include <QList>
#include <QMap>
#include <QRect>
#include <QColor>
#include "TilesetPalette.hpp"
<<<<<<< HEAD
#include "Tile.hpp"

#include "game/io/BaseHdf5IO.hpp"
#include "game/io/TileSetIO.hpp"
#include "game/io/TextureIO.hpp"

=======
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557

// Level canvas for placing tiles
class LevelCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int gridWidth READ gridWidth WRITE setGridWidth NOTIFY gridWidthChanged)
    Q_PROPERTY(int gridHeight READ gridHeight WRITE setGridHeight NOTIFY gridHeightChanged)

public:
<<<<<<< HEAD
    using LevelHdf5IO = jumper::BaseHdf5IO<
        jumper::hdf5features::TextureIO,
        jumper::hdf5features::TileSetIO>;

    explicit LevelCanvas(QQuickItem *parent = nullptr);
    void setExtraTiles(bool mode);

    //Q_INVOKABLE void setTileset(const QImage &path, int tileW = 34, int tileH = 34, int offset = 0, int endIndex = 20);
    Q_INVOKABLE void setTileset(const QList<Tile>& tiles, int tileW, int tileH, int offset, int endIndex);
    
    Q_INVOKABLE void placeTile(int tileIndex);
    Q_INVOKABLE void clearLevel();
    Q_INVOKABLE void saveLevel(const QString &path);
    Q_INVOKABLE void loadLevel(const QString &path);

    int gridWidth() const { return m_gridWidth; }
    void setGridWidth(int w)
    {
        m_gridWidth = w;
        emit gridWidthChanged();
        update();
    }

    int gridHeight() const { return m_gridHeight; }
    void setGridHeight(int h)
    {
        m_gridHeight = h;
        emit gridHeightChanged();
        update();
    }

    void paint(QPainter *painter) override;

signals:

=======
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
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557
    void gridWidthChanged();
    void gridHeightChanged();

protected:
<<<<<<< HEAD
    void mousePressEvent(QMouseEvent *event) override;
=======
    void mousePressEvent(QMouseEvent* event) override;
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557

private:
    QList<Tile> m_tiles;
    QMap<QPair<int, int>, int> m_levelData; // Grid position -> Tile index

<<<<<<< HEAD
    int m_tileWidth = 34;
    int m_tileHeight = 34;
    int m_gridWidth = 20;
    int m_gridHeight = 25;
    int m_currentTileIndex = -1;
    int m_tileOffset = 1;
    QColor m_backgroundColor;

    // ---- Tileset meta ----
    QString m_tilesetPath;        // e.g. "res/tileset1.png"
    QString m_tilesetTextureName; // e.g. "tileset1"

    // Image member
    QImage m_tilesetImage;

=======
    int m_tileWidth = 32;
    int m_tileHeight = 32;
    int m_gridWidth = 20;
    int m_gridHeight = 25;
    int m_currentTileIndex = -1;
    QColor m_backgroundColor;
>>>>>>> 746825d15eebc7077e26ef8a65aa5e78b625d557
    bool m_extraTiles = false;
    int m_endIndex = 20; // at which point do the tiles split up between normal Tiles and extraTiles
};

#endif // LEVELCANVAS_HPP
