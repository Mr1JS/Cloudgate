#ifndef LEVELCANVAS_HPP
#define LEVELCANVAS_HPP

#include <QQuickPaintedItem>
#include <QPixmap>
#include <QList>
#include <QMap>
#include <QRect>
#include <QColor>
#include "TilesetPalette.hpp"
#include "Tile.hpp"

#include "game/io/BaseHdf5IO.hpp"
#include "game/io/TileSetIO.hpp"
#include "game/io/TextureIO.hpp"

// Level canvas for placing tiles
class LevelCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int gridWidth READ gridWidth WRITE setGridWidth NOTIFY gridWidthChanged)
    Q_PROPERTY(int gridHeight READ gridHeight WRITE setGridHeight NOTIFY gridHeightChanged)
    Q_PROPERTY(QString background READ background NOTIFY backgroundChanged)

public:
    using LevelHdf5IO = jumper::BaseHdf5IO<
        jumper::hdf5features::TextureIO,
        jumper::hdf5features::TileSetIO>;

    explicit LevelCanvas(QQuickItem *parent = nullptr);
    void setExtraTiles(bool mode);

    // Q_INVOKABLE void setTileset(const QImage &path, int tileW = 34, int tileH = 34, int offset = 0, int endIndex = 20);
    Q_INVOKABLE void setTileset(const QList<Tile> &tiles, int tileW, int tileH, int offset, int endIndex);

    Q_INVOKABLE void placeTile(int tileIndex);
    Q_INVOKABLE void clearLevel();
    Q_INVOKABLE void saveLevel(const QString &path);
    Q_INVOKABLE void loadLevel(const QString &path);
    Q_INVOKABLE void setQML(QObject *root);
    // Add Rows to Canvas
    Q_INVOKABLE void addRowsAbove(int rows);

    int gridWidth() const { return m_gridWidth; }
    void setGridWidth(int w)
    {
        m_gridWidth = w;
        // Fix
        setWidth(m_gridWidth * m_tileWidth);
        emit gridWidthChanged();
        update();
    }

    int gridHeight() const { return m_gridHeight; }
    void setGridHeight(int h)
    {
        m_gridHeight = h;
        // Fix
        setHeight(m_gridHeight * m_tileHeight);
        emit gridHeightChanged();
        update();
    }

    void paint(QPainter *painter) override;

    // set and get m_background (for Leveleditor.qml)
    void setBackground(QString background);
    QString background();

signals:

    void gridWidthChanged();
    void gridHeightChanged();
    void backgroundChanged(const QString &path);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    int countCoins(const QMap<QPair<int, int>, int> &map);
    void getTilesRules();
    std::array<int, 3> getQMLValues();
    void setQMLValues(std::array<int, 3> qmlValues);
    QList<Tile> m_tiles;
    QMap<QPair<int, int>, int> m_levelData; // Grid position -> Tile index
    QObject *m_qmlRoot;

    int m_tileWidth = 32;
    int m_tileHeight = 32;
    int m_gridWidth = 20;  // not changeable in xml
    int m_gridHeight = 25; // not changeable in xml
    int m_currentTileIndex = -1;
    int m_tileOffset = 4;
    QColor m_backgroundColor;

    // ---- Tileset meta ----
    QString m_tilesetPath = ":/resources/images/tileset.png";
    QString m_tilesetTextureName = "tileset";

    // relative path to the background chosen
    QString m_background = "res/images/backgrounds/mountain.png";

    // Image member
    QImage m_tilesetImage;

    bool m_extraTiles = false;
    int m_endIndex = 127; // at which point do the tiles split up between normal Tiles and extraTiles

    std::map<int, std::string>  m_tileType;

    // method to find if we are in the drawing area
    inline bool isFrameTile(int x, int y) const
    {
        return (x == 0 || x == m_gridWidth - 1 || y == m_gridHeight - 1);
    }
};

#endif // LEVELCANVAS_HPP
