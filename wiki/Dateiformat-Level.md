# Dateiformat Level (XML/HDF5)

## Ueberblick

Ein Level besteht aus zwei Dateien:

- `name.xml` (Metadaten und Referenzen)
- `name.h5` (Tiles und Texturen)

Die XML-Datei enthaelt im Root-Tag:

```xml
<level resources="name.h5">
```

## XML-Struktur (wichtigste Bereiche)

```xml
<level resources="level_01.h5">
  <background_tiles texture="backgroundImg">
    <background_path>res/images/backgrounds/mountain.png</background_path>
    <layer>0</layer>
  </background_tiles>

  <collision_tiles texture="tileset" tiles="level1">
    <tileWidth>32</tileWidth>
    <tileHeight>32</tileHeight>
    <tilesPerRow>...</tilesPerRow>
    <numRows>...</numRows>
    <gridHeight>25</gridHeight>
    <tileOffset>4</tileOffset>
    <endIndex>127</endIndex>
    <layer>1</layer>
  </collision_tiles>

  <heart texture="heart">
    <tileWidth>16</tileWidth>
    <tileHeight>16</tileHeight>
    <layer>3</layer>
  </heart>

  <numbers texture="numbers">
    <num_frames>10</num_frames>
    <frame_width>10</frame_width>
    <frame_height>10</frame_height>
    <layer>3</layer>
  </numbers>

  <actor texture="MariosVergessenerZweiterBruder">
    <num_frames>2</num_frames>
    <frame_width>36</frame_width>
    <frame_height>48</frame_height>
    <position_x>50</position_x>
    <position_y>450</position_y>
    <layer>2</layer>
  </actor>

  <level_forces>
    <gravity_x>0</gravity_x>
    <gravity_y>400</gravity_y>
    <damping_x>0.7</damping_x>
    <damping_y>1.0</damping_y>
    <scrollSpeed>8</scrollSpeed>
  </level_forces>

  <goal>
    <type>0</type>
    <value>0</value>
  </goal>
</level>
```

## HDF5-Struktur

Gruppen:

- `/tiles`
- `/textures`

Wichtige Datasets:

- `/tiles/level1` -> Kollisions-/Gameplay-Tiles (2D int)
- `/textures/tileset` -> Tileset RGBA
- `/textures/backgroundImg` -> Hintergrund
- `/textures/heart`, `/textures/numbers`
- `/textures/<ActorName>` -> Character-Sprites
- `/textures/ghost`, `/textures/snake`

## Index-Konvention

- Im Editor sind Tile-IDs 0-basiert.
- Beim Speichern in HDF5 wird `+1` geschrieben.
- Beim Laden wird wieder `-1` gerechnet.

Das erlaubt `0` als "leer" im gespeicherten Grid.

## Relevante Felder fuer Gameplay

- `goal.type`: 0=None, 1=Coins, 2=Time
- `goal.value`: Zielwert (Coins oder Sekunden)
- `level_forces.scrollSpeed`: Kamera-Scrollgeschwindigkeit
- `actor.texture`: aktiver Character (wird ueber Character-Seite aktualisiert)

