/**
 * @file LevelCanvas.hpp
 * @brief Defines the LevelCanvas class for the visual level editor canvas
 */

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

    /// Width of the grid in tiles (bridge to qml)
    Q_PROPERTY(int gridWidth READ gridWidth WRITE setGridWidth NOTIFY gridWidthChanged)

    /// Height of the grid in tiles (bridge to qml)
    Q_PROPERTY(int gridHeight READ gridHeight WRITE setGridHeight NOTIFY gridHeightChanged)

    /// Path to the background image (bridge to qml)
    Q_PROPERTY(QString background READ background NOTIFY backgroundChanged)

public:

    /// HDF5 I/O operations including textures and tilesets
    using LevelHdf5IO = jumper::BaseHdf5IO<
        jumper::hdf5features::TextureIO,
        jumper::hdf5features::TileSetIO>;

    /// QObject based constructor
    explicit LevelCanvas(QQuickItem *parent = nullptr);

    /// Sets m_extraTiles to know if 1x1 or 1x2 tiles are shown
    void setExtraTiles(bool mode);

    /// Set the current tileset data for the canvas
    Q_INVOKABLE void setTileset(const QList<Tile> &tiles, int tileW, int tileH, int offset, int endIndex);

    /// Place the currently selected tile at a specific index
    Q_INVOKABLE void placeTile(int tileIndex);

    /// Clear all data from the current level
    Q_INVOKABLE void clearLevel();

    /// Save the current level to HDF5 and XML
    Q_INVOKABLE void saveLevel(const QString &path);

    /// Load a level from HDF5 and XML
    Q_INVOKABLE void loadLevel(const QString &path);

    /// Set the QML root object to allow communication (e.g. scroll speed slider)
    Q_INVOKABLE void setQML(QObject *root);

    /// Add rows to canvas
    Q_INVOKABLE void addRowsAbove(int rows);

    /// Remove rows from canvas
    Q_INVOKABLE void removeRowsAbove(int rows);

    /// @return The current grid width
    int gridWidth() const { return m_gridWidth; }

    /// Set new grid width
    void setGridWidth(int w)
    {
        m_gridWidth = w;
        setWidth(m_gridWidth * m_tileWidth);
        emit gridWidthChanged();
        update();
    }

    /// @return The current grid height
    int gridHeight() const { return m_gridHeight; }

    /// Set new grid height
    void setGridHeight(int h)
    {
        m_gridHeight = h;
        setHeight(m_gridHeight * m_tileHeight);
        emit gridHeightChanged();
        update();
    }

    /// Rendering method for the tiles
    void paint(QPainter *painter) override;

    /// Set m_background (for Leveleditor.qml)
    void setBackground(QString background);

    /// @return m_background (for Leveleditor.qml)
    QString background();

signals:

    /// Emit when grid width changes
    void gridWidthChanged();

    /// Emit when grid height changes
    void gridHeightChanged();

    /// Emit when background image changes
    void backgroundChanged(const QString &path);

protected:

    /// Act on mouse Input / check which tile was clicked
    void mousePressEvent(QMouseEvent *event) override;

private:

    /// @return coinAmount (amount of tiles with type collectable)
    int countCoins(const QMap<QPair<int, int>, int> &map);

    /// Get tile definitions from RulesTiles.xml
    void getTilesRules();

    /// @return QMLValues from QML root object (e.g. scroll speed slider)
    std::array<int, 3> getQMLValues();

    /// Set QML Values (when loading a level in the editor)
    void setQMLValues(std::array<int, 3> qmlValues);

    /// Checks if the clicked grid is in the drawing area
    inline bool isFrameTile(int x, int y) const
    {
        return (x == 0 || x == m_gridWidth - 1 || y == m_gridHeight - 1);
    }

    QList<Tile>                 m_tiles; /// List of available tiles
    QMap<QPair<int, int>, int>  m_levelData; /// Grid position -> Tile index
    QObject*                    m_qmlRoot; /// Pointer to QML root object
    int                         m_tileWidth = 32; /// Width of tiles
    int                         m_tileHeight = 32; /// Height of tiles
    int                         m_gridWidth = 20;  /// Width of grid (not changeable in xml)
    int                         m_gridHeight = 25; /// Height of tiles
    int                         m_currentTileIndex = -1; /// Index of currently selected tile
    int                         m_tileOffset = 4; /// Offset between Tiles
    QColor                      m_backgroundColor; /// Background color of tileset to set transparent
    QString                     m_tilesetPath = ":/resources/images/tileset.png"; /// Relative path to tileset for covnerting to hdf5
    QString                     m_tilesetTextureName = "tileset"; /// Name of tileset
    QString                     m_background = "res/images/backgrounds/mountain.png";  /// Relative path to the background chosen
    QImage                      m_tilesetImage; /// Source image of tileset
    bool                        m_extraTiles = false; /// Mode to check if 1x1 or 1x2 tiles are shown
    int                         m_endIndex = 127; /// At which point the tiles split up between normal tiles and extraTiles (end of normal tiles)
    std::map<int, std::string>  m_tileType;  /// Type of Tiles (e.g. collectable)
};

#endif // LEVELCANVAS_HPP
