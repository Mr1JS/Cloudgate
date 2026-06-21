# Spielanleitung

## Menue

Nach dem Start zeigt `Main.qml` drei Hauptpunkte:

- `Start`: Levelauswahl und Spielstart
- `LevelEditor`: Editor zum Erstellen/Bearbeiten von Levels
- `Characters`: Spielfigur auswaehlen

## Level starten

1. `Start` klicken.
2. In `LevelSelector` ein Level aus `res/*.xml` waehlen (ausser `level_master.xml`).
3. Das Spiel wird in `SecondPage.qml` im `GameView` gestartet.

## Steuerung im Spiel

- `Pfeil links/rechts`: Laufen
- `Leertaste`: Springen
- `A`: Figur nach links ausrichten
- `D`: Figur nach rechts ausrichten
- `P`: Pause
- `ESC`: Zurueck zur vorherigen Seite

## Zielsystem

In den Leveldateien gibt es drei Zieltypen:

- `0` (`GOAL_NONE`): Kein spezielles Ziel, Ausgang erreichen.
- `1` (`GOAL_COINS`): Genug Coins sammeln, dann Ausgang nutzen.
- `2` (`GOAL_TIME`): Ausgang vor Ablauf des Timers erreichen.

## Game Over / Level Ende

Game Over bei:

- HP <= 0
- Zeitlimit ueberschritten (bei Zeit-Ziel)
- Spieler faellt in den kritischen unteren Kamerabereich

Level beendet bei:

- Erreichen des Ausgangs mit erfuellter Zielbedingung

## Charakterauswahl

In `Characters`:

- Pfeile links/rechts wechseln die Figur.
- Beim Zurueckgehen (`Back`) wird der Character in allen `res/*.xml` Leveldateien aktualisiert.

