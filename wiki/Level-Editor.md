# Level-Editor

## Ueberblick

Der Editor ist in `qml/LevelEditor.qml` umgesetzt und wird von `LevelEditorController` gesteuert.

Aufbau:

- Links: `TilesetPalette` (Tile-Auswahl)
- Rechts: `LevelCanvas` (Level-Flaeche)
- Oben: Toolbar (Save/Load/Clear/Background/Rows/Goal/ScrollSpeed)

## Grund-Workflow

1. `LevelEditor` im Hauptmenue starten.
2. Tile links auswaehlen.
3. Auf der rechten Canvas platzieren.
4. Mit `Save` speichern (XML + H5).

## Bedienung

- Linksklick auf Canvas: Tile setzen
- Rechtsklick auf Canvas: Tile loeschen
- Rahmen-Tiles (Rand) sind geschuetzt
- Spawnbereich des Spielers (2x2 Feld unten links) ist geschuetzt

## Tile-Sets

Es gibt zwei Modi:

- Normal (`1x1` Tiles)
- Extra (`1x2` Tiles), ueber Switch aktivierbar

Beim 1x2-Modus werden zusammengehoerende Tile-Paare gesetzt/entfernt.

## Grid-Groesse

- `+ 5 Tile above`: fuegt oben 5 Reihen hinzu
- `- 5 Tile above`: entfernt oben Reihen (Minimum Hoehe bleibt 25)

## Hintergrund

`Background` erlaubt die Auswahl eines Bildes aus:

- `res/images/backgrounds/`

Der Hintergrundpfad wird in XML als `background_path` gespeichert.

## Win-Condition und Scroll-Speed

In der Top-Leiste:

- Scroll-Speed (Slider, intern Faktor 4)
- Zieltyp:
  - None
  - Coins (mit Wert)
  - Time (mit Sekundenwert)

Beim Speichern wird bei Coin-Ziel der Wert auf maximal vorhandene Coins begrenzt.

## Save/Load

`Save` erzeugt:

- `levelname.xml`
- `levelname.h5`

`Load` laedt:

- XML-Metadaten (Goal, Tilegroessen, Hintergrund etc.)
- HDF5-Daten (Tiles + Texturen)

