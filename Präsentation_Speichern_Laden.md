# Presentation Script: Saving & Loading in the Game

## 🎮 Overview - What Am I Responsible For?
**"I am responsible for saving and loading levels — both in the Level Editor and in the actual game."**

---

## 📁 What File Formats Do We Use?

We use **2 types of files**:

1. **XML files** (e.g. `level_master.xml`)
   - Store the **configuration**
   - For example: tile size, level dimensions, where are the images located?

2. **HDF5 files** (e.g. `level_master.h5`)
   - Store the **image data** (textures)
   - The actual tile graphics (images for walls, floors, etc.)

---

## 🎨 PART 1: Level Editor — Saving

**"When a designer creates a new level in the editor and wants to save it:"**

### Step by Step:

1. **User clicks "Save"**
   - There is a Save button in the Level Editor

2. **File dialog opens**
   - The user chooses where and under what name to save
   - Default save location: `res/level_master.xml`

3. **Collecting data**
   - The canvas (the drawing area) collects all placed tiles
   - Each tile has a position (X, Y) and a tile ID

4. **Writing the XML file**
   - Saves: level dimensions, tile configuration
   - References the HDF5 file containing the images

5. **Writing the HDF5 file**
   - Saves: the tile arrangement as a numeric grid
   - Each number represents a specific tile

6. **Done!** ✅
   - The user receives a confirmation

---

## 📂 PART 2: Level Editor — Loading

**"When a designer wants to edit an existing level:"**

### Step by Step:

1. **User clicks "Load"**
   - File dialog opens

2. **Select an XML file**
   - The user selects e.g. `level 1.xml`

3. **Reading the XML file**
   - System reads: tile size, tile offset, grid dimensions
   - Determines: which HDF5 file belongs to this level?

4. **Opening the HDF5 file**
   - System loads the tile graphics (images)
   - Loads the tile arrangement (which tile is placed where)

5. **Creating the tileset**
   - All tiles are split into individual images
   - These are loaded into the **palette** (for selection)
   - The level grid is displayed in the **canvas** (for editing)

6. **Done!** ✅
   - The designer can now edit the level

---

## 🎮 PART 3: Game — Loading a Level

**"When a player starts a level:"**

### Step by Step:

1. **Player selects a level**
   - For example "Level 1"

2. **Level constructor is called**
   ```
   Level level(mainWindow, "res/level 1.xml");
   ```

3. **LevelParser reads the files**
   - Opens the XML file
   - Reads all configurations:
     - Where is the player spawn?
     - Where are the enemies?
     - Where are the walls?
     - Where is the goal?

4. **Loading the HDF5 file**
   - Loads the tile graphics
   - Loads the level geometry

5. **Creating game objects**
   - **TileSet**: All walls, floors, platforms
   - **Player** (Actor): At the spawn position
   - **Enemies**: At their respective positions
   - **Physics**: Collision system activated
   - **Camera**: Positioning

6. **Level is ready to play!** 🎮
   - The player can start

---

## 🔄 Summary: The Cycle

```
┌─────────────────┐
│  Level Designer │
│  creates level  │
└────────┬────────┘
         │
         ↓
     [SAVE]
         │
         ↓
┌────────────────────┐
│  XML + HDF5 Files  │
│    on disk         │
└────────┬───────────┘
         │
         ↓
     [LOAD]
         │
    ┌────┴────┐
    │         │
    ↓         ↓
[Editor]   [Game]
 edit it   play it
```

---

## 💡 Why 2 File Formats (XML + HDF5)?

**XML:**
- ✅ Easy to read and edit (text file)
- ✅ Great for configuration
- ❌ Bad for large images

**HDF5:**
- ✅ Very efficient for large amounts of data
- ✅ Perfect for images and arrays
- ❌ Not directly readable (binary format)

**Together = The perfect combination! 🎯**

---

## 🎤 Closing

**"Our save and load system allows:**
- **Designers** to easily create and save levels
- **The game** to load those levels quickly and efficiently
- **Everyone** to work with the same files — the Editor and the Game use the same format!"

---

## ❓ Possible Questions from the Audience

**Q: "Why not store everything in a single file?"**
A: XML is more readable for configuration. HDF5 is more efficient for large amounts of data. Together they give us the best of both worlds.

**Q: "What happens if a file is missing?"**
A: The system shows an error message and does not load the level. There are fallback values where possible.

**Q: "Can players create their own levels?"**
A: Yes! They can use the Level Editor to create and save a level, then play it in the Game — it uses the same files!

---

## 🎯 Key Classes (Technical)

In case of technical questions:

- **LevelEditorController**: Coordinates saving/loading in the editor
- **LevelCanvas**: Manages the level grid and writes data
- **Level**: Represents a playable level
- **LevelParser**: Reads XML and HDF5 for the game
- **BaseHdf5IO**: Low-level HDF5 implementation

---

**Good luck with your presentation! 🚀**
