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

```text
project1_c++/
├── CMakeLists.txt
├── README.md
├── src/                    # Qt/C++-Anwendung und QML-Bridge (GameView, Controller, main.cpp)
├── include/                # Header der Qt/C++-Anwendung
├── qml/                    # QML-Seiten und UI-Komponenten
├── game/                   # Spiel-Engine (aus uebung10)
│   ├── src/                # Engine-Implementierung (Physik, Kamera, Level, Rendering)
│   ├── include/            # Engine-Header
│   ├── io/                 # CRTP-basierte I/O-Frameworks zum Speichern und Laden
│   ├── Vector.tcc
│   └── TileTree.tcc
├── res/                    # Assets und Leveldaten
│   ├── images/             # Sprites, UI-Assets, Hintergründe, Actor-Bilder
│   ├── tileDefinition/     # Tile-Regeln (XML)
│   ├── *.xml               # Level-Beschreibungen
│   └── *.h5                # Leveldaten (HDF5)
├── cmake/                  # Zusätzliche CMake-Find-Module
├── ext/HighFive/           # Externe HDF5-Bibliothek (Vendor-Code)
└── build/                  # Build-Artefakte (lokal erzeugt)
```

## Abhängigkeiten

### System-Abhängigkeiten

- **Qt 6.8.x** (Quick, Widgets, Quick Controls)
- **SDL2** und **SDL2_image**
- **HDF5** (C, CXX, HL Komponenten)
- **Boost** (filesystem, system)
- **Box2D**

### ⚠️ Qt-Version (Wichtig!)

Dieses Projekt erfordert **Qt 6.8.x**.

Viele Linux-Distributionen (z. B. Ubuntu 22.04) liefern standardmäßig ältere Qt-6-Versionen (z. B. 6.2 oder 6.4). Diese Versionen können zu Build- oder QML-Problemen führen.

#### Installierte Version prüfen

```bash
qmake6 --version
# oder
qtpaths6 --version
```

Falls die Version nicht **6.8.x** ist, muss Qt 6.8 manuell installiert werden.

---

### Installation von Qt 6.8 (offizieller Weg)

#### 1. Qt Online Installer herunterladen

Besuche: https://www.qt.io/download

#### 2. Im Installer auswählen

- **Qt → 6.8.x → Desktop gcc 64-bit**
- Qt Quick
- Qt Quick Controls

Standardmäßiger Installationspfad: `~/Qt/6.8.x/gcc_64`

#### 3. Environment korrekt setzen

Falls Qt außerhalb der Systempfade installiert ist (z. B. `~/Qt/6.8.x/gcc_64`), muss die Umgebung angepasst werden.

**Temporär (für aktuelle Shell):**
```bash
export Qt6_DIR=~/Qt/6.8.x/gcc_64/lib/cmake/Qt6
export PATH=~/Qt/6.8.x/gcc_64/bin:$PATH
export LD_LIBRARY_PATH=~/Qt/6.8.x/gcc_64/lib:$LD_LIBRARY_PATH
```

**Permanent (empfohlen):**

In `~/.bashrc` oder `~/.zshrc` hinzufügen:
```bash
export Qt6_DIR=$HOME/Qt/6.8.x/gcc_64/lib/cmake/Qt6
export PATH=$HOME/Qt/6.8.x/gcc_64/bin:$PATH
export LD_LIBRARY_PATH=$HOME/Qt/6.8.x/gcc_64/lib:$LD_LIBRARY_PATH
```

Danach:
```bash
source ~/.bashrc
```

---

### Installation weiterer Abhängigkeiten (Ubuntu/Debian)

> ⚠️ **Hinweis:** Qt sollte **nicht** über `apt install qt6-*` installiert werden, wenn explizit Qt 6.8 benötigt wird.

```bash
sudo apt update
sudo apt install -y \
    libsdl2-dev \
    libsdl2-image-dev \
    libboost-all-dev \
    libhdf5-dev \
    libbox2d-dev
```

## Kompilierung

### 1. Build-Verzeichnis erstellen

```bash
cd project1
mkdir -p build
cd build
```

### 2. CMake konfigurieren

**Standard (Qt im System-Pfad):**
```bash
cmake ..
```

**Falls Qt manuell installiert wurde (z.B. Qt 6.8.x):**
```bash
cmake -B . -S .. -DCMAKE_PREFIX_PATH=$HOME/Qt/6.8.x/gcc_64
```

#### MacOS (Eine Option auswählen)
Dieser Befehl dient dazu, die Qt-Version zu ermitteln, wenn diese bei der Standardversion nicht gefunden werden kann. In diesem Fall muss der `PATH` manuell eingegeben werden. 

Option 1:
```bash
cmake -S .. -B . \
  -DCMAKE_PREFIX_PATH="~/Qt/6.10.1/macos"
```

Option 2:
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

## Bekannte Probleme / Einschränkungen

- Das Rendering kopiert bei jedem Frame die Pixel (kann bei hohen Auflösungen langsam sein)
- SDL2-Tastatur-Events müssen manuell von Qt konvertiert werden
- Das versteckte SDL2-Fenster wird trotzdem erstellt (benötigt für den Renderer)
- Die Spiel-Größe ist fest auf 800x600 Pixel eingestellt
- Die Kamera scrollt nur vertikal (feste X-Position)

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

