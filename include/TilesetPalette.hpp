/**
 * @file TilesetPalette.hpp
 * @brief Defines the TilesetPalette class for managing tile selection in the level editor
 */

#ifndef TILESETPALETTE_HPP
#define TILESETPALETTE_HPP

#include <QQuickPaintedItem>
#include <QPixmap>
#include <QList>
#include <QRect>
#include <QColor>
#include <QMap>
#include "Tile.hpp"

// Tileset palette for tile selection
class TilesetPalette : public QQuickPaintedItem
{
    Q_OBJECT

    /// Total number of tiles (bridge to qml)
    Q_PROPERTY(int tileCount READ tileCount NOTIFY tileCountChanged)

public:

    /// QObject based Constructor
    explicit TilesetPalette(QQuickItem *parent = nullptr);

    /// Set tileset with all the important data
    Q_INVOKABLE void setTileset(const QList<Tile>& tiles, int tileW = 32, int tileH = 32, int offset = 4, int endIndex = 127);

    /// Load the tile names from a rules.xml file
    void loadTileNames(const QString &xmlPath);

    /// @return total number of tiles
    int tileCount() const { return m_tiles.size(); }

    /// Sets m_extraTiles to know if 1x1 or 1x2 tiles are shown
    void setExtraTiles(bool mode);

    /// Rendering method for the tiles
    void paint(QPainter *painter) override;

signals:

    /// Emit when a tile is selected / clicked
    void tileSelected(int index);

    /// Emit when a new number of tiles exists (new tileset)
    void tileCountChanged();

protected:

    /// Act on mouse Input / check which tile was clicked
    void mousePressEvent(QMouseEvent *event) override;

private:
    QList<Tile>         m_tiles; /// List of tiles
    QMap<int, QString>  m_tileNames; /// ID -> Name mapping
    int                 m_tileWidth = 32; /// Width of the tiles in the tileset
    int                 m_tileHeight = 32; /// Height of the tiles in the tileset
    int                 m_tileOffset = 4; /// Offset between tiles in the tileset
    int                 m_selectedIndex = -1; /// Index of selected tile
    int                 m_spacing = 6; /// Space between tiles in the grid
    QColor              m_backgroundColor; /// Color (our tileset: RGB(92, 130, 161)) that is treated as transparent
    bool                m_extraTiles = false; /// Mode to check if 1x1 or 1x2 tiles are shown
    int                 m_endIndex = 127; /// At which point the tiles split up between normal tiles and extraTiles (end of normal tiles)
};

#endif // TILESETPALETTE_HPP
