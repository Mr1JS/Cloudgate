# Installation und Start

## Voraussetzungen

- CMake >= 3.5
- C++ Compiler mit C++17/20 Support
- Qt6 (Quick + Widgets)
- SDL2 und SDL2_image
- HDF5 (C, CXX, HL)
- Boost (filesystem)
- Box2D

## Linux (Ubuntu/Debian) - Pakete

```bash
sudo apt update
sudo apt install -y \
  qt6-base-dev \
  qt6-declarative-dev \
  qml6-module-qtquick-controls \
  qml6-module-qtquick-templates \
  qml6-module-qtqml-workerscript \
  qml6-module-qtquick-window \
  qml6-module-qtquick-layouts \
  libsdl2-dev \
  libsdl2-image-dev \
  libboost-all-dev \
  libhdf5-dev \
  libbox2d-dev
```

## Build

```bash
cd project1
mkdir -p build
cd build
cmake ..
cmake --build . -j
```

macOS (Qt-Pfad ggf. anpassen):

```bash
cmake -S .. -B . -DCMAKE_PREFIX_PATH="$HOME/Qt/6.10.1/macos"
cmake --build . -j
```

## Starten

```bash
cd project1/build
cp -r ../res ./
./appCloudgate_game
```

Hinweis: Je nach System kann die Binary in einem App-Bundle liegen.

## Typische Probleme

1. Ressourcen fehlen beim Start:
   `res/` muss im Build-Ordner verfuegbar sein.
2. QML-Module fehlen:
   fehlende `qml6-module-*` Pakete installieren.
3. SDL/Rendering instabil auf Linux:
   das Projekt setzt auf Linux bereits `QSG_RENDER_LOOP=basic` und `JUMPER_SDL_SOFTWARE=1`, falls nicht vorhanden.

