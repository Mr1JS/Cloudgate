# Technische Architektur

## Verzeichnisstruktur (vereinfacht)

- `src/`, `include/`: Qt/QML-Anbindung, Editor-Logik, GameView
- `qml/`: UI-Seiten (Main, LevelEditor, LevelSelector, Character, SecondPage)
- `game/src/`, `game/include/`: Engine (Level, Physics, Camera, Actor, Parser)
- `res/`: XML/HDF5 Leveldaten und Assets
- `ext/HighFive/`: HDF5 C++ Helper

## Hauptkomponenten

1. UI-Schicht (Qt/QML)
- Navigation per `StackView` (`Main.qml`)
- Spielseite `SecondPage.qml`
- Editorseite `LevelEditor.qml`

2. Glue-Schicht (Qt C++)
- `GameView`: SDL-Engine im QML-Item
- `LevelEditorController`: verbindet Palette und Canvas
- `LevelSelector`: findet Level-XMLs und liest Goal-Metadaten
- `ActorSelector`: Charakterwahl und Update aller Leveldateien

3. Engine-Schicht (`jumper` Namespace)
- `MainWindow`: SDL-Fenster/Renderer (hidden/offscreen)
- `Level`: Gameplay-Zustand, Kamera, Goal-State
- `Physics`: Kollisionen, Bewegung, Monster/Doors
- `LevelParser`: liest XML und HDF5, erstellt Spielobjekte

## Render- und Inputfluss

1. `GameView::startGame()` erstellt `jumper::MainWindow`.
2. QTimer (`~60 FPS`) ruft `GameView::updateGame()`.
3. Qt-Keyevents werden in SDL-Scancodes gemappt.
4. Engine aktualisiert Level/Physik.
5. SDL rendert auf hidden/offscreen target.
6. `SDL_RenderReadPixels()` kopiert nach `QImage`.
7. `QPainter` zeichnet das Bild in QML.

## Editor-Datenfluss

1. `LevelEditorController::loadTileset()` liest XML und HDF5-Textur.
2. Tiles werden an `TilesetPalette` und `LevelCanvas` verteilt.
3. `LevelCanvas` verwaltet `m_levelData` (Grid -> TileIndex).
4. `saveLevel()` schreibt XML + HDF5.
5. `loadLevel()` liest XML + HDF5 und rekonstruiert Grid/UI-Werte.

## Besondere Implementierungsdetails

- SDL wird mit verstecktem Fenster betrieben (`SDL_WINDOW_HIDDEN`).
- Linux-Fallback auf stabilen Renderpfad (software/basic).
- Tile-Indizes werden beim Speichern in `level1` um +1 verschoben.
- Charakterauswahl schreibt den `actor texture="..."` Eintrag in alle Level-XMLs.

