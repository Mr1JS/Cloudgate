#ifndef LEVELEDITORCONTROLLER_HPP
#define LEVELEDITORCONTROLLER_HPP

#include <QObject>
#include <QString>

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
    Q_PROPERTY(QString currentTilesetPath READ currentTilesetPath NOTIFY tilesetPathChanged)
    Q_PROPERTY(int selectedTileIndex READ selectedTileIndex NOTIFY selectedTileChanged)

public:
    explicit LevelEditorController(QObject* parent = nullptr);

    // Tileset management
    Q_INVOKABLE void loadTileset(const QString& path, int tileWidth = 32, int tileHeight = 32, int offset = 4, int endIndex = 20);
    Q_INVOKABLE void setTileDimensions(int width, int height);

    // Level management
    Q_INVOKABLE void newLevel(int gridWidth = 20, int gridHeight = 25);
    Q_INVOKABLE void clearLevel();
    Q_INVOKABLE void saveLevel(const QString& path);
    Q_INVOKABLE void loadLevel(const QString& path);
    Q_INVOKABLE void toggleExtraTileset(bool enabled);

    // Tile selection
    Q_INVOKABLE void selectTile(int tileIndex);

    // Registration methods for components
    Q_INVOKABLE void registerPalette(TilesetPalette* palette);
    Q_INVOKABLE void registerCanvas(LevelCanvas* canvas);

    // Property getters
    QString currentTilesetPath() const { return m_tilesetPath; }
    int selectedTileIndex() const { return m_selectedTileIndex; }

signals:
    void tilesetPathChanged();
    void selectedTileChanged();
    void levelCleared();
    void levelSaved(const QString& path);
    void levelLoaded(const QString& path);

private:
    TilesetPalette* m_palette = nullptr;
    LevelCanvas* m_canvas = nullptr;
    QString m_tilesetPath;
    int m_tileWidth = 32;
    int m_tileHeight = 32;
    int m_tileOffset = 4;
    int m_selectedTileIndex = -1;

    int m_gridWidth = 20;
    int m_gridHeight = 25;
    int m_endIndex = 20; // at which point do the tiles split up between normal Tiles and extraTiles
};

#endif // LEVELEDITORCONTROLLER_HPP
