/**
 * @file LevelEditorController.hpp
 * @brief Defines the LevelEditorController class for managing level editor operations
 */

#ifndef LEVELEDITORCONTROLLER_HPP
#define LEVELEDITORCONTROLLER_HPP

#include <QObject>
#include <QList>
#include <QString>
#include "Tile.hpp"


// Forward declarations
class TilesetPalette;
class LevelCanvas;

/**
 * @brief Controller class that manages the Level Editor
 *
 * This class acts as the "brain" coordinating between
 * TilesetPalette and LevelCanvas components.
 */
class LevelEditorController : public QObject
{
    Q_OBJECT

    /// Path to the currently loaded tileset (bridge to qml)
    Q_PROPERTY(QString currentTilesetPath READ currentTilesetPath NOTIFY tilesetPathChanged)

    /// Index of the currently selected tile (bridge to qml)
    Q_PROPERTY(int selectedTileIndex READ selectedTileIndex NOTIFY selectedTileChanged)

public:

    /// QObject based constructor
    explicit LevelEditorController(QObject *parent = nullptr);

    /// Load a tileset from the given file path
    Q_INVOKABLE void loadTileset(const QString &path);

    /// Initialize a new level with grid dimensions
    Q_INVOKABLE void newLevel(int gridWidth = 20, int gridHeight = 25);

    /// Clear all data from the current level
    Q_INVOKABLE void clearLevel();

    /// Save without parameters - opens QFileDialog internally
    Q_INVOKABLE void saveLevel();

    /// Load without parameters - opens QFileDialog internally
    Q_INVOKABLE void loadLevel();

    /// Load background - opens QFileDialog internally
    Q_INVOKABLE void loadBackground();

    /// Toggle visibility of extra tileset tiles
    Q_INVOKABLE void toggleExtraTileset(bool enabled);

    /// Update the currently selected tile index
    Q_INVOKABLE void selectTile(int tileIndex);

    /// Register the palette component
    Q_INVOKABLE void registerPalette(TilesetPalette *palette);

    /// Register the canvas component
    Q_INVOKABLE void registerCanvas(LevelCanvas *canvas);

    /// Add empty rows to the top of the level canvas
    Q_INVOKABLE void addRowsAbove(int rows);

    /// Remove rows from the top of the level canvas
    Q_INVOKABLE void removeRowsAbove(int rows);

    /// @return The path to the current tileset
    QString currentTilesetPath() const { return m_tilesetPath; }

    /// @return The index of the selected tile
    int selectedTileIndex() const { return m_selectedTileIndex; }

signals:

    /// Emit when tileset path changes
    void tilesetPathChanged();

    /// Emit when the selected tile index changes
    void selectedTileChanged();

    /// Emit when the level is cleared
    void levelCleared();

    /// Emit when a level is successfully saved
    void levelSaved(const QString &path);

    /// Emit when a level is successfully loaded
    void levelLoaded(const QString &path);

    /// Emit when tileset data is ready
    void tilesetReady(const QList<Tile>& tiles, int tileW, int tileH, int offset, int endIndex);

private:

    TilesetPalette* m_palette = nullptr; /// Pointer to the registered palette
    LevelCanvas*    m_canvas = nullptr; /// Pointer to the registered canvas
    QString         m_tilesetPath; /// Path to the tileset
    int             m_tileWidth = 32; /// Width of the tiles
    int             m_tileHeight = 32; /// Height of the tiles
    int             m_tileOffset = 4; /// Offset between the tiles
    int             m_selectedTileIndex = -1; /// Index of tile currently selected
    int             m_gridWidth = 20; /// Width of grid (not changeable in xml)
    int             m_gridHeight = 25; /// Height of tiles
    int             m_endIndex = 127; /// At which point the tiles split up between normal tiles and extraTiles (end of normal tiles)
};

#endif // LEVELEDITORCONTROLLER_HPP
