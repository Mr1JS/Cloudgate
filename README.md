# Cloudgate Game - Project 1

A Qt/QML-based game with an integrated physics engine, level system, and camera control from uebung10. The game runs directly in the Qt window, not in a separate SDL2 window.

## Overview

This project combines a modern Qt/QML user interface with the complete game engine from uebung10. The game is rendered directly in the Qt window and offers:

- **Physics Engine**: Full collision detection and physics simulation
- **Level System**: Loading levels from HDF5 files
- **Camera System**: Automatic camera that follows the player
- **Actor System**: Animated player character with jump and movement mechanics
- **Tile System**: Rendering of background and collision tiles
- **SDL2 Integration**: SDL2 is used with a hidden window and embedded in Qt/QML

## Project Structure

```text
project1_c++/
├── CMakeLists.txt
├── README.md
├── src/                    # Qt/C++ application and QML bridge (GameView, Controller, main.cpp)
├── include/                # Qt/C++ application headers
├── qml/                    # QML pages and UI components
├── game/                   # Game engine (from uebung10)
│   ├── src/                # Engine implementation (physics, camera, level, rendering)
│   ├── include/            # Engine headers
│   ├── io/                 # CRTP-based I/O frameworks for saving and loading
│   ├── Vector.tcc
│   └── TileTree.tcc
├── res/                    # Assets and level data
│   ├── images/             # Sprites, UI assets, backgrounds, actor images
│   ├── tileDefinition/     # Tile rules (XML)
│   ├── *.xml               # Level descriptions
│   └── *.h5                # Level data (HDF5)
├── cmake/                  # Additional CMake find modules
├── ext/HighFive/           # External HDF5 library (vendor code)
└── build/                  # Build artifacts (generated locally)
```

## Dependencies

### System Dependencies

- **Qt 6.8.x** (Quick, Widgets, Quick Controls)
- **SDL2** and **SDL2_image**
- **HDF5** (C, CXX, HL components)
- **Boost** (filesystem, system)
- **Box2D**

### ⚠️ Qt Version (Important!)

This project requires **Qt 6.8.x**.

Many Linux distributions (e.g. Ubuntu 22.04) ship older Qt 6 versions by default (e.g. 6.2 or 6.4). These versions can cause build or QML issues.

#### Check Installed Version

```bash
qmake6 --version
# or
qtpaths6 --version
```

If the version is not **6.8.x**, Qt 6.8 must be installed manually.

---

### Installing Qt 6.8 (Official Method)

#### 1. Download the Qt Online Installer

Visit: https://www.qt.io/download

#### 2. Select in the Installer

- **Qt → 6.8.x → Desktop gcc 64-bit**
- Qt Quick
- Qt Quick Controls

Default installation path: `~/Qt/6.8.x/gcc_64`

#### 3. Set the Environment Correctly

If Qt is installed outside the system paths (e.g. `~/Qt/6.8.x/gcc_64`), the environment must be adjusted.

**Temporarily (for current shell):**
```bash
export Qt6_DIR=~/Qt/6.8.x/gcc_64/lib/cmake/Qt6
export PATH=~/Qt/6.8.x/gcc_64/bin:$PATH
export LD_LIBRARY_PATH=~/Qt/6.8.x/gcc_64/lib:$LD_LIBRARY_PATH
```

**Permanently (recommended):**

Add to `~/.bashrc` or `~/.zshrc`:
```bash
export Qt6_DIR=$HOME/Qt/6.8.x/gcc_64/lib/cmake/Qt6
export PATH=$HOME/Qt/6.8.x/gcc_64/bin:$PATH
export LD_LIBRARY_PATH=$HOME/Qt/6.8.x/gcc_64/lib:$LD_LIBRARY_PATH
```

Then run:
```bash
source ~/.bashrc
```

---

### Installing Further Dependencies (Ubuntu/Debian)

> ⚠️ **Note:** Qt should **not** be installed via `apt install qt6-*` if Qt 6.8 is explicitly required.

```bash
sudo apt update
sudo apt install -y \
    libsdl2-dev \
    libsdl2-image-dev \
    libboost-all-dev \
    libhdf5-dev \
    libbox2d-dev
```

## Building

### 1. Create Build Directory

```bash
cd project1
mkdir -p build
cd build
```

### 2. Configure CMake

**Standard (Qt in system path):**
```bash
cmake ..
```

**If Qt was installed manually (e.g. Qt 6.8.x):**
```bash
cmake -B . -S .. -DCMAKE_PREFIX_PATH=$HOME/Qt/6.8.x/gcc_64
```

#### macOS (Choose one option)
This command is used to detect the Qt version if it cannot be found at the default location. In that case the `PATH` must be entered manually.

Option 1:
```bash
cmake -S .. -B . \
  -DCMAKE_PREFIX_PATH="~/Qt/6.10.1/macos"
```

Option 2:
```bash
~/Qt/6.10.1/macos/bin/qt-cmake -S . -B build -DCMAKE_PREFIX_PATH=~/Qt/6.10.1/macos
```

### 3. Compile

```bash
make
```

### 4. Run

```bash
# Copy resources to build directory
cp -r ../res ./

# Run the game
./appCloudgate_game
```

## Documentation

The project uses **Doxygen** for automatic API documentation generation.

### Install Doxygen

**Windows:**
```powershell
choco install doxygen.install graphviz
```

**Ubuntu/Debian:**
```bash
sudo apt install doxygen graphviz
```

**macOS:**
```bash
brew install doxygen graphviz
```

### Generate Documentation

```bash
cd build
cmake ..  # (if not already run)
cmake --build . --target doc
```

The HTML documentation is generated in `build/docs/html/`. Open `build/docs/html/index.html` or `build/docs/html/files.html` in your browser.

### What Gets Documented?

Doxygen automatically extracts:
- All classes and their hierarchy
- Functions and methods with signatures
- Member variables
- Inheritance relationships
- Call graphs (who calls which function)
- Include dependencies
- Class diagrams (UML-style)

Documentation is generated automatically, even without special comments in the code.

## Usage

### Main Menu

After launching, the main menu appears with three options:

1. **Start**: Opens the game page with the embedded game
2. **LevelEditor**: Opens the level editor
3. **Setting**: Opens the settings

### Game Controls

- **Arrow Keys Left/Right**: Move left/right
- **Space**: Jump
- **A/D**: Flip the player character
- **ESC**: Return to main menu

### Game Window

When you click "Start", the game page opens. The game runs directly in the Qt window (800×600 pixels) and is rendered in real time. There are no separate windows anymore.

## Known Issues / Limitations

- The renderer copies pixels every frame (can be slow at high resolutions)
- SDL2 keyboard events must be manually converted from Qt
- The hidden SDL2 window is still created (required for the renderer)
- The game size is fixed at 800×600 pixels
- The camera only scrolls vertically (fixed X position)

## Troubleshooting

### Game Does Not Start

- Check whether the level files (`res/level.xml`, `res/level.h5`) exist
- Check the debug output for path information
- Make sure SDL2 was initialized correctly

### No Keyboard Input

- Make sure `GameView` has focus (`setFocus(true)`)
- Check the `convertQtKeyToSDL()` method for key mapping

### Rendering Issues

- Check whether `SDL_RenderReadPixels()` succeeds
- Make sure the renderer was initialized correctly
- Check the pixel format conversion

## License

See the LICENSE file for details.
