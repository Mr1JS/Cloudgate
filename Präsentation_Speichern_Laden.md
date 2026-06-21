# Präsentationsskript: Speichern & Laden im Game

## 🎮 Überblick - Was mache ich?
**"Ich bin für das Speichern und Laden von Levels zuständig - sowohl im Level-Editor als auch im eigentlichen Spiel."**

---

## 📁 Welche Dateiformate nutzen wir?

Wir verwenden **2 Arten von Dateien**:

1. **XML-Dateien** (z.B. `level_master.xml`)
   - Speichern die **Konfiguration**
   - Zum Beispiel: Tile-Größe, Level-Dimensionen, wo liegen die Bilder?
   
2. **HDF5-Dateien** (z.B. `level_master.h5`)
   - Speichern die **Bilddaten** (Textures)
   - Die eigentlichen Tile-Grafiken (Bilder für Wände, Boden, etc.)

---

## 🎨 TEIL 1: Level-Editor - Speichern

**"Wenn ein Designer ein neues Level im Editor erstellt und speichern möchte:"**

### Schritt-für-Schritt:

1. **Benutzer klickt auf "Speichern"**
   - Im Level-Editor gibt es einen Speichern-Button

2. **File-Dialog öffnet sich**
   - Der Benutzer wählt wo und unter welchem Namen gespeichert wird
   - Standard-Speicherort: `res/level_master.xml`

3. **Daten sammeln**
   - Das Canvas (die Zeichenfläche) sammelt alle platzierten Tiles
   - Jedes Tile hat eine Position (X, Y) und eine Tile-ID

4. **XML-Datei schreiben**
   - Speichert: Level-Dimensionen, Tile-Konfiguration
   - Verweist auf die HDF5-Datei mit den Bildern

5. **HDF5-Datei schreiben**
   - Speichert: Die Tile-Anordnung als Zahlengitter
   - Jede Zahl steht für ein bestimmtes Tile

6. **Fertig!** ✅
   - Der Benutzer bekommt eine Bestätigung

---

## 📂 TEIL 2: Level-Editor - Laden

**"Wenn ein Designer ein existierendes Level bearbeiten möchte:"**

### Schritt-für-Schritt:

1. **Benutzer klickt auf "Laden"**
   - File-Dialog öffnet sich

2. **XML-Datei auswählen**
   - Der Benutzer wählt z.B. `level 1.xml`

3. **XML-Datei lesen**
   - System liest: Tile-Größe, Tile-Offset, Grid-Dimensionen
   - Findet heraus: Welche HDF5-Datei gehört dazu?

4. **HDF5-Datei öffnen**
   - System lädt die Tile-Grafiken (Bilder)
   - Lädt die Tile-Anordnung (welches Tile wo platziert ist)

5. **Tileset erstellen**
   - Alle Tiles werden in einzelne Bilder zerlegt
   - Diese werden in die **Palette** geladen (zur Auswahl)
   - Das Level-Grid wird im **Canvas** angezeigt (zum Bearbeiten)

6. **Fertig!** ✅
   - Der Designer kann jetzt das Level bearbeiten

---

## 🎮 TEIL 3: Game - Level laden

**"Wenn ein Spieler ein Level startet:"**

### Schritt-für-Schritt:

1. **Spieler wählt ein Level**
   - Zum Beispiel "Level 1"

2. **Level-Konstruktor wird aufgerufen**
   ```
   Level level(mainWindow, "res/level 1.xml");
   ```

3. **LevelParser liest die Dateien**
   - Öffnet die XML-Datei
   - Liest alle Konfigurationen:
     - Wo ist der Spieler-Start?
     - Wo sind Monster?
     - Wo sind Wände?
     - Wo ist das Ziel?

4. **HDF5-Datei laden**
   - Lädt die Tile-Grafiken
   - Lädt die Level-Geometrie

5. **Spielelemente erstellen**
   - **TileSet**: Alle Wände, Böden, Plattformen
   - **Spieler** (Actor): An der Start-Position
   - **Monster**: An ihren Positionen
   - **Physik**: Kollisions-System aktivieren
   - **Kamera**: Positionierung

6. **Level ist spielbereit!** 🎮
   - Der Spieler kann loslegen

---

## 🔄 Zusammenfassung: Der Kreislauf

```
┌─────────────────┐
│  Level-Designer │
│  erstellt Level │
└────────┬────────┘
         │
         ↓
    [SPEICHERN]
         │
         ↓
┌────────────────────┐
│  XML + HDF5 Files  │
│   auf Festplatte   │
└────────┬───────────┘
         │
         ↓
     [LADEN]
         │
    ┌────┴────┐
    │         │
    ↓         ↓
[Editor]  [Game]
bearbeiten spielen
```

---

## 💡 Warum 2 Dateiformate (XML + HDF5)?

**XML:**
- ✅ Einfach zu lesen und bearbeiten (Text-Datei)
- ✅ Gut für Konfiguration
- ❌ Schlecht für große Bilder

**HDF5:**
- ✅ Sehr effizient für große Datenmengen
- ✅ Perfekt für Bilder und Arrays
- ❌ Nicht direkt lesbar (Binär-Format)

**Zusammen = Perfekte Kombination! 🎯**

---

## 🎤 Abschluss

**"Unser Speicher- und Lade-System ermöglicht es:**
- **Designern**, Levels einfach zu erstellen und zu speichern
- **Dem Spiel**, diese Levels schnell und effizient zu laden
- **Allen**, mit den gleichen Dateien zu arbeiten - Editor und Game nutzen das gleiche Format!"

---

## ❓ Mögliche Fragen aus dem Publikum

**Q: "Warum nicht alles in einer Datei speichern?"**
A: XML ist für Konfiguration besser lesbar. HDF5 ist für große Datenmengen effizienter. Beide zusammen geben uns Flexibilität.

**Q: "Was passiert, wenn eine Datei fehlt?"**
A: Das System zeigt eine Fehlermeldung und lädt das Level nicht. Es gibt Fallback auf Standard-Werte wo möglich.

**Q: "Können Spieler ihre eigenen Levels erstellen?"**
A: Ja! Sie können den Level-Editor nutzen, speichern, und dann im Game spielen - es verwendet die gleichen Dateien!

---

## 🎯 Die wichtigsten Klassen (Technisch)

Falls technische Fragen kommen:

- **LevelEditorController**: Koordiniert Speichern/Laden im Editor
- **LevelCanvas**: Verwaltet das Level-Grid und schreibt Daten
- **Level**: Repräsentiert ein spielbares Level
- **LevelParser**: Liest XML und HDF5 für das Game
- **BaseHdf5IO**: Technische HDF5-Implementierung

---

**Viel Erfolg bei deiner Präsentation! 🚀**
