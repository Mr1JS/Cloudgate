# Cloudgate Game - Projekt 1

Ein Qt/QML-basiertes Spiel mit integrierter Physik-Engine, Level-System und Camera-Steuerung aus uebung10. Das Spiel läuft direkt im Qt-Fenster, nicht in einem separaten SDL2-Fenster.

## Übersicht

Dieses Projekt kombiniert eine moderne Qt/QML-Benutzeroberfläche mit der vollständigen Spiel-Engine aus uebung10. Das Spiel wird direkt im Qt-Fenster gerendert und bietet:

- **Physik-Engine**: Vollständige Kollisionserkennung und Physik-Simulation
- **Level-System**: Laden von Leveln aus HDF5-Dateien
- **Camera-System**: Automatische Camera, die dem Spieler folgt
- **Actor-System**: Animierte Spielfigur mit Sprung- und Bewegungsmechanik
- **Tile-System**: Rendering von Hintergrund- und Kollisions-Tiles
- **SDL2-Integration**: SDL2 wird mit verstecktem Fenster verwendet und in Qt/QML eingebettet

## Projektstruktur

> Being edited, Comming Soon!

## Abhängigkeiten

### System-Abhängigkeiten

- **Qt6** (Quick-Modul)
- **SDL2** und **SDL2_image**
- **HDF5** (C, CXX, HL Komponenten)
- **Boost** (filesystem, system)

### Installation der Abhängigkeiten (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y \
    qt6-base-dev \
    qt6-declarative-dev \
    libsdl2-dev \
    libsdl2-image-dev \
    libhdf5-dev \
    libboost-system-dev \
    libboost-filesystem-dev
```

## Kompilierung

### 1. Build-Verzeichnis erstellen

```bash
cd project1
mkdir -p build
cd build
```

### 2. CMake konfigurieren

```bash
cmake ..
```

for macos
```bash
cmake -S .. -B . \
  -DCMAKE_PREFIX_PATH="~/Qt/6.10.1/macos"
```

```bash
~/Qt/6.10.1/macos/bin/qt-cmake -S . -B build -DCMAKE_PREFIX_PATH=~/Qt/6.10.1/macos
```

### 3. Kompilieren

```bash
make
```

### 4. Ausführen

```bash
# Copy resources to build directory
cp -r ../res ./

# Run the game
./appCloudgate_game
```

## Verwendung

### Hauptmenü

Nach dem Start erscheint das Hauptmenü mit drei Optionen:

1. **Start**: Öffnet die Spiel-Seite mit dem eingebetteten Spiel
2. **LevelEditor**: Öffnet den Level-Editor
3. **Setting**: Öffnet die Einstellungen

### Spiel-Steuerung

- **Pfeiltasten Links/Rechts**: Bewegung nach links/rechts
- **Leertaste**: Springen
- **A/D**: Spielfigur umdrehen
- **ESC**: Zurück zum Hauptmenü

### Spiel-Fenster

Wenn du auf "Start" klickst, öffnet sich die Spiel-Seite. Das Spiel läuft direkt im Qt-Fenster (800x600 Pixel) und wird in Echtzeit gerendert. Es gibt keine separaten Fenster mehr.

## Technische Implementierung

### SDL2-Integration in Qt/QML

Das Projekt verwendet einen innovativen Ansatz, um SDL2 direkt in Qt/QML einzubetten:

#### 1. Verstecktes SDL2-Fenster

Die `MainWindow`-Klasse wurde modifiziert, um ein verstecktes SDL2-Fenster zu erstellen:

```cpp
// In src/game/MainWindow.cpp
m_window = SDL_CreateWindow(
    "Jumper Main Window",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    m_width,
    m_height,
    SDL_WINDOW_HIDDEN  // Verstecktes Fenster!
);
```

#### 2. GameView - QML-Integration

Die `GameView`-Klasse (`GameView.hpp`/`GameView.cpp`) ist ein `QQuickPaintedItem`, das:

- SDL2 mit verstecktem Fenster initialisiert
- Den SDL2-Renderer-Inhalt in eine `QImage` kopiert
- Diese `QImage` in QML rendert
- Tastatur-Events von Qt zu SDL konvertiert

**Wichtigste Methoden:**

```cpp
class GameView : public QQuickPaintedItem
{
    // Rendert SDL2-Inhalt in QML
    void paint(QPainter *painter) override;
    
    // Startet das Spiel
    void startGame();
    
    // Stoppt das Spiel
    void stopGame();
    
    // Update-Loop (60 FPS)
    void updateGame();
    
    // Konvertiert Qt-Tastatur-Events zu SDL
    void keyPressEvent(QKeyEvent *event) override;
    SDL_Keycode convertQtKeyToSDL(int qtKey);
};
```

#### 3. Rendering-Pipeline

Das Rendering funktioniert folgendermaßen:

1. **SDL2-Rendering**: Das Spiel rendert in den SDL2-Renderer (verstecktes Fenster)
2. **Pixel-Kopie**: `SDL_RenderReadPixels()` kopiert die Pixel in eine `SDL_Surface`
3. **QImage-Konvertierung**: Die `SDL_Surface` wird zu einer `QImage` konvertiert
4. **QML-Darstellung**: Die `QImage` wird im `paint()`-Handler von `QQuickPaintedItem` gezeichnet

**Code-Ausschnitt aus GameView.cpp:**

```cpp
void GameView::paint(QPainter *painter)
{
    // Render game to SDL renderer
    m_gameWindow->render();
    
    // Copy SDL renderer content to QImage
    SDL_Surface* surface = SDL_CreateRGBSurface(0, m_gameWidth, m_gameHeight, 32,
                                                  0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    
    // Read pixels from renderer
    SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, 
                        surface->pixels, surface->pitch);
    
    // Convert SDL surface to QImage
    QImage image(static_cast<uchar*>(surface->pixels), 
                 surface->w, surface->h, surface->pitch, 
                 QImage::Format_ARGB32);
    
    // Draw image to painter
    painter->drawImage(boundingRect(), image.copy());
    SDL_FreeSurface(surface);
}
```

#### 4. Tastatur-Event-Weiterleitung

Qt-Tastatur-Events werden zu SDL-Events konvertiert:

```cpp
void GameView::keyPressEvent(QKeyEvent *event)
{
    SDL_Event sdlEvent;
    sdlEvent.type = SDL_KEYDOWN;
    sdlEvent.key.keysym.sym = convertQtKeyToSDL(event->key());
    SDL_PushEvent(&sdlEvent);
}

SDL_Keycode GameView::convertQtKeyToSDL(int qtKey)
{
    switch (qtKey) {
        case Qt::Key_Left: return SDLK_LEFT;
        case Qt::Key_Right: return SDLK_RIGHT;
        case Qt::Key_Space: return SDLK_SPACE;
        case Qt::Key_A: return SDLK_a;
        case Qt::Key_D: return SDLK_d;
        default: return SDLK_UNKNOWN;
    }
}
```

#### 5. MainWindow-Erweiterungen

Die `MainWindow`-Klasse wurde um folgende Methoden erweitert:

```cpp
class MainWindow {
    // Manuelles Update (ohne run()-Loop)
    void update(const Uint8* keystates);
    
    // Manuelles Rendering
    void render();
    
    // Zugriff auf Level
    Level* level();
};
```

Diese Methoden ermöglichen es, das Spiel außerhalb der `run()`-Methode zu steuern.

### Update-Loop

Der Update-Loop läuft über einen `QTimer` mit 60 FPS (16ms Intervall):

```cpp
m_updateTimer = new QTimer(this);
connect(m_updateTimer, &QTimer::timeout, this, &GameView::updateGame);
m_updateTimer->start(16); // 60 FPS
```

In `updateGame()`:
1. SDL-Events werden verarbeitet
2. Tastatur-Zustand wird abgerufen
3. `MainWindow::update()` wird aufgerufen
4. `update()` wird aufgerufen, um `paint()` zu triggern

## Implementierte Features

### Von uebung10 integriert

1. **Physik-System** (`src/game/Physics.*`)
   - Kollisionserkennung mit Tiles
   - Gravitation und Dämpfung
   - Sprung-Mechanik mit maximaler Sprunghöhe
   - Geschwindigkeitsbegrenzungen

2. **Level-System** (`src/game/Level.*`)
   - Laden von Leveln aus XML/HDF5-Dateien
   - Unterstützung für Hintergrund- und Kollisions-Tiles
   - Layer-Management für Rendering-Reihenfolge

3. **Camera-System** (`src/game/Camera.*`)
   - Automatisches Folgen der Spielfigur (nur vertikal)
   - Feste X-Position: Die Kamera scrollt nur nach oben/unten, nicht horizontal
   - Welt-zu-Bildschirm-Koordinaten-Transformation
   - Kamera-Position kann in `Level.cpp` angepasst werden

4. **Actor-System** (`src/game/Actor.*`)
   - Animierte Spielfigur
   - Physikalische Eigenschaften (Kräfte, Geschwindigkeiten)
   - Sprung- und Bewegungslogik

5. **Tile-System** (`src/game/TileSet.*`)
   - Rendering von Tile-Maps
   - Kollisionserkennung basierend auf Tile-Indizes
   - Unterstützung für verschiedene Tile-Sets

### Qt/QML Integration

- **GameView**: `QQuickPaintedItem`, das SDL2 direkt in QML einbettet
- **SecondPage.qml**: QML-Seite mit eingebettetem Spiel
- **Tastatur-Event-Handling**: Automatische Konvertierung von Qt- zu SDL-Events
- **Rendering-Pipeline**: SDL2 → QImage → QML

## QML-Verwendung

### GameView in QML

```qml
import Cloudgate_game 1.0

GameView {
    id: gameView
    width: 800
    height: 600
    
    onGameStarted: {
        console.log("Game started")
    }
    
    onGameStopped: {
        console.log("Game stopped")
    }
    
    // Spiel starten
    Component.onCompleted: {
        gameView.startGame()
    }
}
```

### Properties

- `levelPath` (QString): Pfad zur Level-XML-Datei
- `running` (bool): Ob das Spiel läuft (read-only)

### Signals

- `gameStarted()`: Wird emittiert, wenn das Spiel startet
- `gameStopped()`: Wird emittiert, wenn das Spiel stoppt
- `levelPathChanged()`: Wird emittiert, wenn der Level-Pfad geändert wird
- `runningChanged()`: Wird emittiert, wenn der Running-Status sich ändert

### Slots

- `startGame()`: Startet das Spiel
- `stopGame()`: Stoppt das Spiel

## Level-Dateien

Level werden aus zwei Dateien geladen:

- **level.xml**: XML-Definition mit Level-Konfiguration (Tiles, Actor, Kräfte)
- **level.h5**: HDF5-Datei mit binären Daten (Texturen, Tile-Maps)

Die Level-Dateien befinden sich im `res/`-Verzeichnis.

### Level-Pfad-Auflösung

Der `GameView` löst den Level-Pfad automatisch auf:

1. Zuerst wird relativ zum ausführbaren Programm gesucht (ein Verzeichnis nach oben, dann `res/level.xml`)
2. Falls nicht gefunden, wird relativ zum aktuellen Arbeitsverzeichnis gesucht
3. Falls ein absoluter Pfad angegeben wurde, wird dieser verwendet

## Build-System

Das Projekt verwendet CMake mit:
- Qt6-Integration für QML
- SDL2 für das Spiel-Rendering (verstecktes Fenster)
- HDF5 für Level-Daten
- Boost für Dateisystem-Operationen

### Wichtige CMake-Konfigurationen

```cmake
# SDL2 finden
find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED)

# HDF5 finden
find_package(HDF5 REQUIRED COMPONENTS C CXX HL)

# Boost finden
find_package(Boost 1.54 COMPONENTS filesystem system REQUIRED)

# GameView registrieren
qmlRegisterType<GameView>("Cloudgate_game", 1, 0, "GameView");
```

## Entwicklung

### Hinzufügen neuer Level

1. Erstelle eine neue `level.xml`-Datei im `res/`-Verzeichnis
2. Erstelle die entsprechende `level.h5`-Datei mit den Level-Daten
3. Ändere den `levelPath` in `SecondPage.qml` oder setze ihn programmatisch

### Anpassen der Spiel-Physik

Die Physik-Parameter können in der `level.xml` angepasst werden:
- `gravity_x`, `gravity_y`: Gravitationskräfte
- `damping_x`, `damping_y`: Dämpfung
- `jump_force_y`: Sprungkraft
- `max_run_velocity`: Maximale Laufgeschwindigkeit
- `max_fall_velocity`: Maximale Fallgeschwindigkeit
- `max_jump_height`: Maximale Sprunghöhe

### Anpassen der Spiel-Größe

Die Spiel-Größe ist fest auf 800x600 Pixel eingestellt. Um sie zu ändern:

1. Ändere `m_gameWidth` und `m_gameHeight` in `GameView.cpp`
2. Passe die Größe in `SecondPage.qml` an

### Anpassen der Kamera-Position

Die Kamera-Position kann in `src/game/Level.cpp` angepasst werden:

```cpp
Level::Level(MainWindow* mainWindow, std::string filename)
    : StaticRenderable(mainWindow),
      m_mainWindow(mainWindow),
      m_camera(400, 600, mainWindow->w(), mainWindow->h()),  // X, Y, Breite, Höhe
      m_layers(&m_camera)
```

- **X-Position**: Horizontale Startposition der Kamera (Standard: 400)
- **Y-Position**: Vertikale Startposition der Kamera (Standard: 600)
- Die Kamera scrollt nur vertikal (feste X-Position), folgt dem Spieler aber in Y-Richtung

### Anpassen der Y-Position (Kamera, Spieler, Karte)

Um alles weiter unten zu positionieren, können folgende Positionen angepasst werden:

1. **Kamera Y-Position**: In `src/game/Level.cpp` Zeile 34, zweiter Parameter
2. **Spieler Y-Position**: In `res/level.xml`, `<position_y>` Tag
3. **Karte Y-Offset**: In `src/game/TileSet.cpp` Zeile 164, `target.y` Berechnung (+600 Offset)

**Beispiel:**
- Kamera: `m_camera(400, 600, ...)` → Y=600
- Spieler: `<position_y>600</position_y>` → Y=600
- Karte: `target.y = ... + 600` → Y-Offset von 600

Die Kollisionserkennung in `Physics.cpp` wurde entsprechend angepasst, um den Y-Offset zu berücksichtigen.

### Performance-Optimierungen

Das Rendering kopiert bei jedem Frame die Pixel vom SDL2-Renderer. Mögliche Optimierungen:

1. **Textur-basiertes Rendering**: Verwende `SDL_Texture` statt `SDL_Surface`
2. **Framerate-Limitierung**: Reduziere die Update-Rate bei niedriger Priorität
3. **Pufferung**: Puffere das gerenderte Bild und aktualisiere nur bei Änderungen

## Bekannte Probleme / Einschränkungen

- Das Rendering kopiert bei jedem Frame die Pixel (kann bei hohen Auflösungen langsam sein)
- SDL2-Tastatur-Events müssen manuell von Qt konvertiert werden
- Das versteckte SDL2-Fenster wird trotzdem erstellt (benötigt für den Renderer)
- Die Spiel-Größe ist fest auf 800x600 Pixel eingestellt
- Die Kamera scrollt nur vertikal (feste X-Position)

## Sicherheits-Fixes

### Grenzprüfungen für Tile-Zugriffe

Um Speicherzugriffsfehler zu vermeiden, wurden Grenzprüfungen für negative Tile-Koordinaten hinzugefügt:

1. **TileArray::get()** (`src/game/TileArray.cpp`): Prüft jetzt auch auf negative Werte (`x >= 0 && y >= 0`)
2. **TileTree::get()** (`src/game/TileTree.tcc`): Prüft jetzt auch auf negative `pos` Werte (`pos >= 0 && pos < m_numTiles`)
3. **Physics::resolveCollision()** (`src/game/Physics.cpp`): Prüft jetzt auch auf negative Werte für `f_x`, `f_y`, `d_x`, `d_y`

Diese Fixes verhindern Speicherzugriffsfehler, wenn der Spieler außerhalb des gültigen Tile-Bereichs läuft.

### Level-Grenzen als Kollisionsflächen

Die linken und rechten Ränder des Levels fungieren jetzt als Kollisionsflächen:

- **Linke Grenze**: Der Spieler kann nicht links aus dem Level laufen (X < 0)
- **Rechte Grenze**: Der Spieler kann nicht rechts aus dem Level laufen (X + Sprite-Breite > Level-Breite)
- **Geschwindigkeit stoppen**: Wenn der Spieler gegen eine Wand läuft, wird die horizontale Geschwindigkeit auf 0 gesetzt

Implementiert in `Physics::resolveCollision()` (`src/game/Physics.cpp`).

## Troubleshooting

### Spiel startet nicht

- Überprüfe, ob die Level-Dateien (`res/level.xml`, `res/level.h5`) existieren
- Überprüfe die Debug-Ausgaben für Pfad-Informationen
- Stelle sicher, dass SDL2 korrekt initialisiert wurde

### Keine Tastatur-Eingabe

- Stelle sicher, dass das `GameView` den Fokus hat (`setFocus(true)`)
- Überprüfe die `convertQtKeyToSDL()`-Methode für die Tasten-Mapping

### Rendering-Probleme

- Überprüfe, ob `SDL_RenderReadPixels()` erfolgreich ist
- Stelle sicher, dass der Renderer korrekt initialisiert wurde
- Überprüfe die Pixel-Format-Konvertierung

## Lizenz

Siehe LICENSE-Datei für Details.

## Autoren

- Projekt 1: Qt/QML-Integration, SDL2-Einbettung
- uebung10: Spiel-Engine (Physik, Level, Camera, etc.)

## Changelog

### Version 0.3
- **ESC-Taste**: Führt jetzt direkt zurück zum Hauptmenü (statt Pause-Menü)
- **Kamera-Verhalten**: Kamera hat jetzt eine feste X-Position und scrollt nur vertikal (oben/unten)
- **Kamera-Position**: Kann in `Level.cpp` angepasst werden (Standard: X=400, Y=600)
- **Level-Grenzen**: Linke und rechte Ränder des Levels fungieren jetzt als Kollisionsflächen
- **Y-Position Anpassungen**: Kamera, Spieler und Karte können weiter unten positioniert werden
- **Sicherheits-Fixes**: Grenzprüfungen für negative Tile-Koordinaten hinzugefügt, um Speicherzugriffsfehler zu vermeiden
  - `TileArray::get()` prüft jetzt auf negative Werte
  - `TileTree::get()` prüft jetzt auf negative Indizes
  - `Physics::resolveCollision()` prüft jetzt auf negative Tile-Koordinaten
  - `Physics::resolveCollision()` prüft jetzt Level-Grenzen (links/rechts) und stoppt den Spieler

### Version 0.2
- **SDL2-Einbettung**: Spiel läuft jetzt direkt im Qt-Fenster, nicht mehr in separatem Fenster
- **GameView**: Neue Klasse zum Einbetten von SDL2 in QML
- **Rendering-Pipeline**: SDL2 → QImage → QML
- **Tastatur-Event-Weiterleitung**: Automatische Konvertierung von Qt- zu SDL-Events
- **MainWindow-Erweiterungen**: Neue Methoden für manuelles Update/Rendering

### Version 0.1
- Initiale Integration der Spiel-Engine aus uebung10
- Qt/QML-Benutzeroberfläche
- GameController für Spiel-Verwaltung (separates Fenster)
- Level-Laden aus HDF5-Dateien
