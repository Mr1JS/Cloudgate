#include "include/EndlessLevelGenerator.hpp"
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <QDebug>
#include <QSaveFile>
#include <QTextStream>
#include <QFileInfo>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

#include <H5Lpublic.h>
#include <ext/HighFive/include/highfive/H5DataSet.hpp>
#include <ext/HighFive/include/highfive/H5DataSpace.hpp>
#include <ext/HighFive/include/highfive/H5File.hpp>

static QString findResPath()
{
    QStringList candidates;
    QDir appDir(QCoreApplication::applicationDirPath());

    // Executable in build/ oder build/appCloudgate_game/
    QDir d1 = appDir;
    if (d1.cdUp())
        candidates << d1.absoluteFilePath("res");
    QDir d2 = appDir;
    if (d2.cdUp() && d2.cdUp())
        candidates << d2.absoluteFilePath("res");

    QDir cur = QDir::current();
    candidates << cur.absoluteFilePath("res");
    candidates << "res";

    for (const QString& p : candidates) {
        if (QFile::exists(p + "/level_master.h5") || QFile::exists(p + "/level 1.h5"))
            return QDir::cleanPath(p);
    }
    return candidates.first();
}

EndlessLevelGenerator::EndlessLevelGenerator(QObject* parent)
    : QObject(parent)
{
    m_resPath = findResPath();
    qDebug() << "EndlessLevelGenerator: res path =" << m_resPath
             << "level_master.h5 exists:" << QFile::exists(m_resPath + "/level_master.h5");
}

void EndlessLevelGenerator::setLastLevelType(const QString& type)
{
    if (m_lastLevelType != type) {
        m_lastLevelType = type;
        emit lastLevelTypeChanged();
    }
}

QString EndlessLevelGenerator::generateLevel(int levelNumber)
{
    const int GRID_W = 20;   // ~640px
    const int EXTRA_TOP_ROWS = 12;   // Leere Reihen oben – Wände bis ganz oben
    const int GRID_H = 25 + EXTRA_TOP_ROWS;   // 37 Reihen
    const int TILE_EMPTY = 0;
    const int TILE_DOOR_TOP = 128;   // Door Open Top (1-based: RulesTiles id 127)
    const int TILE_DOOR_MID = 129;   // Door Open Mid (1-based: RulesTiles id 128)
    const int TILE_DOOR_CLOSED_TOP = 130;  // Door Closed Top (RulesTiles id 129)
    const int TILE_DOOR_CLOSED_MID = 131; // Door Closed Mid (RulesTiles id 130)
    const int TILE_KEY = 120;        // Key Yellow (RulesTiles id 119)
    const int TILE_COIN = 119;       // Coin Gold (118+1)
    const int TILE_WALL = 85;        // Dirt Cliff Right – Worldborders
    const int TILE_SPIKES = 114;     // Spikes (113+1)
    const int TILE_RED_POTION = 125;   // Red Potion / Lebensrank (124+1)
    const int TILE_BLUE_POTION = 126;  // Blue Potion / Super-Trank (125+1)
    const int TILE_GREEN_POTION = 127; // Green Potion / Break-Tiles (126+1)
    const int TILE_GHOST_TOP = 132;

    const int TILE_GHOST_BOTTOM = 133;
    const int TILE_SNAKE_TOP = 134;
    const int TILE_SNAKE_BOTTOM = 135;

    // Boden-Tiles: Sand(1), Snow(19), Grass(37), Stone(55), Dirt(73), Castle(91) – 1-based
    static const int GROUND_TILES[] = {1, 19, 37, 55, 73, 91};
    static const int NUM_GROUND = sizeof(GROUND_TILES) / sizeof(GROUND_TILES[0]);
    auto pickGroundType = []() { return GROUND_TILES[std::rand() % NUM_GROUND]; };

    std::vector<std::vector<int>> grid(GRID_H, std::vector<int>(GRID_W, TILE_EMPTY));

    // Worldborders links und rechts – durchgehend bis ganz oben
    for (int r = 0; r < GRID_H; ++r) {
        grid[r][0] = TILE_WALL;
        grid[r][GRID_W - 1] = TILE_WALL;
    }

    // Zufallsseed
    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        seeded = true;
    }

    // Spawn-Zone: Radius 3 Tiles – keine Plattform dort
    const int spawnCol = 1;           // position_x=50 → ~Tile 1
    const int spawnRow = GRID_H - 2; // Oberkante Boden
    const int SPAWN_RADIUS = 3;

    const int SPAWN_RESERVE_COLS = 5;
    // Tür pro bestandenem Level 5 Tiles höher (schwieriger) – mind. Row 2 (Tür braucht 3 Reihen)
    const int doorY = std::max(2, EXTRA_TOP_ROWS - 5 * (levelNumber - 1));
    struct Step { int row, xStart, width; };
    std::vector<Step> path;

    // Master nur für Level 1 – ab Level 2 prozedural (Tür höher, Plattformen müssen reichen)
    QString masterH5 = m_resPath + "/level_master.h5";
    if (!QFile::exists(masterH5))
        masterH5 = m_resPath + "/level 1.h5";
    if (levelNumber == 1 && QFile::exists(masterH5)) {
        try {
            HighFive::File masterFile(masterH5.toStdString(), HighFive::File::ReadOnly);
            if (masterFile.exist("tiles/level1")) {
                auto dset = masterFile.getDataSet("tiles/level1");
                auto dims = dset.getSpace().getDimensions();
                int masterH = static_cast<int>(dims[0]);
                int masterW = static_cast<int>(dims.size() > 1 ? dims[1] : GRID_W);
                std::vector<int> masterData(masterH * masterW);
                dset.read(masterData.data());
                // Master in untere Reihen kopieren (Row 0 = oben im Master)
                int dstStart = GRID_H - masterH;
                for (int mr = 0; mr < masterH && dstStart + mr < GRID_H; ++mr)
                    for (int c = 0; c < std::min(masterW, GRID_W); ++c) {
                        int v = masterData[mr * masterW + c];
                        if (v != 0) grid[dstStart + mr][c] = v;
                    }
                // Plattform-Pfad aus Master extrahieren (Oberkanten: Reihe darüber leer)
                for (int r = dstStart + 1; r < GRID_H - 1; ++r) {
                    int start = -1, end = -1;
                    for (int c = 1; c < GRID_W - 1; ++c) {
                        if (grid[r][c] != TILE_EMPTY && grid[r+1][c] != TILE_EMPTY &&
                            grid[r-1][c] == TILE_EMPTY) {
                            if (start < 0) start = c;
                            end = c;
                        }
                    }
                    if (start >= 0 && end >= start)
                        path.push_back({r, start, end - start + 1});
                }
                // Spawn-Zone freiräumen (keine Plattform im Radius 3)
                for (int sr = std::max(0, spawnRow - SPAWN_RADIUS); sr < spawnRow; ++sr)
                    for (int sc = std::max(1, spawnCol - SPAWN_RADIUS); sc <= std::min(GRID_W - 2, spawnCol + SPAWN_RADIUS); ++sc)
                        if (grid[sr][sc] != TILE_WALL) grid[sr][sc] = TILE_EMPTY;
            }
        } catch (const std::exception& e) {
            qWarning() << "EndlessLevelGenerator: master load failed:" << e.what();
        }
    }

    // Plattformen: Breite 3–5 Tiles, Zickzack (nie direkt übereinander), vertikal Step 2 oder 3
    if (path.empty()) {
        int floorType = pickGroundType();
        for (int x = 1; x < GRID_W - 1; ++x) {
            grid[GRID_H - 1][x] = floorType;
            grid[GRID_H - 2][x] = floorType;
        }
        const int WIDTH_MIN = 5, WIDTH_MAX = 7;   // Plattform-Breite 5–7 Tiles
        const int STEP_MIN = 2, STEP_MAX = 3;     // Vertikaler Abstand: 2 oder 3 Reihen
        const int GAP_MIN = 1, GAP_MAX = 3;       // Horizontaler Abstand im Zickzack (keine Überlappung)
        int r = GRID_H - 2 - (STEP_MIN - 1);     // Erste Plattform 2 Tiles näher am Boden
        int xStart = 2;
        int platformIndex = 0;
        while (r >= doorY) {
            int width = WIDTH_MIN + (std::rand() % (WIDTH_MAX - WIDTH_MIN + 1));
            xStart = std::max(1, std::min(xStart, GRID_W - 1 - width));
            int step = STEP_MIN + (std::rand() % (STEP_MAX - STEP_MIN + 1));
            // Spawn-Zone meiden: keine Plattform im Radius 3
            bool inSpawnZone = (r >= spawnRow - SPAWN_RADIUS && r <= spawnRow + SPAWN_RADIUS) &&
                (xStart <= spawnCol + SPAWN_RADIUS && xStart + width - 1 >= spawnCol - SPAWN_RADIUS);
            if (inSpawnZone) {
                r -= 1 + step;
                // Zickzack: nächste Plattform mit Abstand (nicht übereinander)
                int gap = GAP_MIN + (std::rand() % (GAP_MAX - GAP_MIN + 1));
                if (platformIndex % 2 == 0) xStart += width + gap;   // Rechts
                else {
                    xStart -= width + gap;   // Links
                    if (xStart < 1) xStart = 1 + (std::rand() % (GRID_W - width - 2));
                }
                ++platformIndex;
                continue;
            }
            int platformType = pickGroundType();
            path.push_back({r, xStart, width});
            for (int x = 0; x < width && xStart + x < GRID_W - 1; ++x)
                grid[r][xStart + x] = platformType;
            r -= 1 + step;   // 1 Reihe Plattform + vertikaler Abstand (2 oder 3)
            // Zickzack: nächste Plattform links/rechts versetzt mit Abstand (nie direkt übereinander)
            int gap = GAP_MIN + (std::rand() % (GAP_MAX - GAP_MIN + 1));
            if (platformIndex % 2 == 0)
                xStart += width + gap;   // Rechts
            else {
                xStart -= width + gap;   // Links
                if (xStart < 1) xStart = 1 + (std::rand() % (GRID_W - width - 2));
            }
            ++platformIndex;
        }
    }

    // Tür: zufällig 1 von 3 Arten – Normal, Schlüssel, Münzen (2–5)
    int doorType = std::rand() % 3;   // 0=normal, 1=schlüssel, 2=münzen
    bool useClosedDoor = (doorType >= 1);
    int goalType = useClosedDoor ? 1 : 0;   // 1 = COINS
    int goalValue = 0;
    if (doorType == 1) goalValue = 1;       // Schlüssel zählt als 1 Münze
    else if (doorType == 2) goalValue = 2 + (std::rand() % 4);   // 2–5 Münzen

    if (!path.empty()) {
        auto topIt = std::min_element(path.begin(), path.end(),
            [](const Step& a, const Step& b) { return a.row < b.row; });
        const Step& topPlatform = *topIt;
        int doorRow = topPlatform.row;
        int doorCol = std::max(1, std::min(GRID_W - 2, topPlatform.xStart + topPlatform.width / 2));
        int doorPlatformType = pickGroundType();
        // Alte Tür-Reste (vom Master) entfernen
        for (int sr = 0; sr < GRID_H; ++sr)
            for (int sc = 1; sc < GRID_W - 1; ++sc)
                if (grid[sr][sc] == TILE_DOOR_TOP || grid[sr][sc] == TILE_DOOR_MID
                    || grid[sr][sc] == TILE_DOOR_CLOSED_TOP || grid[sr][sc] == TILE_DOOR_CLOSED_MID)
                    grid[sr][sc] = TILE_EMPTY;
        if (doorRow >= 2) {
            int topTile = useClosedDoor ? TILE_DOOR_CLOSED_TOP : TILE_DOOR_TOP;
            int midTile = useClosedDoor ? TILE_DOOR_CLOSED_MID : TILE_DOOR_MID;
            grid[doorRow - 2][doorCol] = topTile;
            grid[doorRow - 1][doorCol] = midTile;
        }
        for (int dx = -2; dx <= 2; ++dx)
            if (doorCol + dx >= 0 && doorCol + dx < GRID_W)
                grid[doorRow][doorCol + dx] = doorPlatformType;   // Plattform unter Tür
    }

    // Schlüssel bei Schlüssel-Tür: auf einer Plattform platzieren (nicht auf der Tür-Plattform)
    if (doorType == 1 && path.size() >= 2) {
        for (int attempt = 0; attempt < 30; ++attempt) {
            int idx = 1 + (std::rand() % (path.size() - 1));   // Mittlere Plattformen, nicht die oberste
            const Step& s = path[idx];
            int kx = s.xStart + s.width / 2;
            if (kx < 1 || kx >= GRID_W - 1) continue;
            int ky = s.row - 1;
            if (ky < 0 || grid[ky][kx] != TILE_EMPTY || grid[ky + 1][kx] == TILE_EMPTY) continue;
            grid[ky][kx] = TILE_KEY;
            break;
        }
    }

    // Münzen: bei Normal-Tür 5 Stück, bei Schlüssel-Tür 0, bei Münzen-Tür genau goalValue
    int numCoins = (doorType == 0) ? 5 : (doorType == 1) ? 0 : goalValue;
    if (numCoins > 0 && !path.empty()) {
        // Gültige Positionen sammeln (über Plattform-Oberkanten)
        std::vector<std::pair<int, int>> validPositions;
        for (const Step& s : path) {
            int cy = s.row - 1;
            if (cy < 0) continue;
            for (int cx = s.xStart; cx < s.xStart + s.width && cx < GRID_W - 1; ++cx) {
                if (cx < 1) continue;
                if (grid[cy][cx] == TILE_EMPTY && grid[cy + 1][cx] != TILE_EMPTY)
                    validPositions.push_back({cy, cx});
            }
        }
        // Zufällig numCoins Positionen auswählen und Münzen platzieren
        int toPlace = std::min(numCoins, (int)validPositions.size());
        for (int i = 0; i < toPlace; ++i) {
            int j = i + (std::rand() % ((int)validPositions.size() - i));
            std::swap(validPositions[i], validPositions[j]);
        }
        for (int i = 0; i < toPlace; ++i)
            grid[validPositions[i].first][validPositions[i].second] = TILE_COIN;
    }

    // Gegner (Ghost/Snake) auf Plattform-Oberkanten – 2 Tiles hoch
    for (const Step& s : path) {
        int r = s.row;
        if (r < 2 || r >= GRID_H - 2) continue;
        if (std::rand() % 3 != 0) continue;   // ~33% Chance pro Plattform
        int gx = 3 + (std::rand() % (GRID_W - 8));
        if (gx <= SPAWN_RESERVE_COLS || grid[r][gx] == TILE_EMPTY) continue;
        if (grid[r - 1][gx] != TILE_EMPTY || grid[r - 2][gx] != TILE_EMPTY) continue;
        bool ghost = (std::rand() % 2 == 0);
        grid[r - 2][gx] = ghost ? TILE_GHOST_TOP : TILE_SNAKE_TOP;
        grid[r - 1][gx] = ghost ? TILE_GHOST_BOTTOM : TILE_SNAKE_BOTTOM;
    }

    // Spikes mittig auf Plattformen
    for (int i = 0; i < 3 && path.size() >= 3; ++i) {
        const Step& s = path[2 + (std::rand() % std::min(6, (int)path.size() - 2))];   // Mittlere Plattformen
        int cx = s.xStart + s.width / 2;   // Mitte der Plattform (nicht am Rand)
        if (cx < 1 || cx >= GRID_W - 1) continue;
        int cy = s.row - 1;
        if (cy < 0 || grid[cy][cx] != TILE_EMPTY || grid[cy + 1][cx] == TILE_EMPTY) continue;
        grid[cy][cx] = TILE_SPIKES;
    }

    // Tränke: je max 1, jeweils ~50% Chance
    auto tryPlacePotion = [&](int tileId) {
        if (std::rand() % 2 != 0) return;
        for (int attempt = 0; attempt < 20; ++attempt) {
            int cx = 4 + (std::rand() % (GRID_W - 10));
            int cy = EXTRA_TOP_ROWS + 3 + (std::rand() % (GRID_H - EXTRA_TOP_ROWS - 10));
            if (cx >= 0 && cx < GRID_W && cy >= 0 && cy < GRID_H - 1 &&
                grid[cy][cx] == TILE_EMPTY && grid[cy + 1][cx] != TILE_EMPTY) {
                grid[cy][cx] = tileId;
                break;
            }
        }
    };
    tryPlacePotion(TILE_RED_POTION);    // Lebensrank
    tryPlacePotion(TILE_BLUE_POTION);   // Super-Trank
    tryPlacePotion(TILE_GREEN_POTION);  // Break-Tiles

    // Spawn-Position: Spieler immer auf dem Boden (Tile Y = (GRID_H-2)*32+600, Actor-Höhe 48)
    // LevelParser: actor_y = pos_y + 32*(gridH+2) → pos_y = floorTop - 48 - 32*(GRID_H+2)
    const int TILE_H = 32;
    const int TILE_Y_OFFSET = 600;
    const int ACTOR_HEIGHT = 48;
    int floorTopY = (GRID_H - 2) * TILE_H + TILE_Y_OFFSET;
    int spawnY = floorTopY - ACTOR_HEIGHT - TILE_H * (GRID_H + 2);

    // H5: level_master.h5 als Vorlage, sonst level 1.h5
    QString srcH5 = m_resPath + "/level_master.h5";
    if (!QFile::exists(srcH5))
        srcH5 = m_resPath + "/level 1.h5";
    QString dstH5 = m_resPath + "/endless.h5";
    if (!QFile::exists(srcH5)) {
        qWarning() << "EndlessLevelGenerator: no level H5 found at" << m_resPath;
        return QString();
    }
    QFile::remove(dstH5);
    if (!QFile::copy(srcH5, dstH5)) {
        qWarning() << "EndlessLevelGenerator: failed to copy H5 file";
        return QString();
    }

    // Tiles-Dataset in endless.h5 überschreiben
    try {
        HighFive::File file(dstH5.toStdString(), HighFive::File::ReadWrite);
        HighFive::Group tilesGroup = file.getGroup("tiles");

        if (tilesGroup.exist("level1")) {
            H5Ldelete(tilesGroup.getId(), "level1", H5P_DEFAULT);
        }

        std::vector<size_t> dims = {static_cast<size_t>(GRID_H), static_cast<size_t>(GRID_W)};
        int data[GRID_H][GRID_W];
        for (int r = 0; r < GRID_H; ++r)
            for (int c = 0; c < GRID_W; ++c)
                data[r][c] = grid[r][c];

        HighFive::DataSpace ds(dims);
        HighFive::DataSet dset = tilesGroup.createDataSet<int>("level1", ds);
        dset.write(data);  // 2D C-Array – HighFive erkennt 2 Dimensionen
    } catch (const std::exception& e) {
        qWarning() << "EndlessLevelGenerator: HDF5 error:" << e.what();
        return QString();
    }

    // endless.xml schreiben
    QString xmlPath = m_resPath + "/endless.xml";
    QSaveFile xmlFile(xmlPath);
    if (!xmlFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "EndlessLevelGenerator: cannot write endless.xml";
        return QString();
    }
    QTextStream out(&xmlFile);
    out << "<level resources=\"endless.h5\">\n"
        << "  <background_tiles texture=\"backgroundImg\">\n"
        << "    <background_path>res/images/backgrounds/mountain.png</background_path>\n"
        << "    <layer>0</layer>\n"
        << "  </background_tiles>\n"
        << "  <collision_tiles texture=\"tileset\" tiles=\"level1\">\n"
        << "    <tileWidth>32</tileWidth>\n"
        << "    <tileHeight>32</tileHeight>\n"
        << "    <tilesPerRow>9</tilesPerRow>\n"
        << "    <numRows>15</numRows>\n"
        << "    <gridHeight>" << GRID_H << "</gridHeight>\n"
        << "    <tileOffset>4</tileOffset>\n"
        << "    <switchIndex>127</switchIndex>\n"
        << "    <layer>1</layer>\n"
        << "  </collision_tiles>\n"
        << "  <heart texture=\"heart\">\n"
        << "    <tileWidth>16</tileWidth>\n"
        << "    <tileHeight>16</tileHeight>\n"
        << "    <layer>3</layer>\n"
        << "  </heart>\n"
        << "  <numbers texture=\"numbers\">\n"
        << "    <num_frames>10</num_frames>\n"
        << "    <frame_width>10</frame_width>\n"
        << "    <frame_height>10</frame_height>\n"
        << "    <layer>3</layer>\n"
        << "  </numbers>\n"
        << "  <actor texture=\"Wiepower\">\n"
        << "    <num_frames>2</num_frames>\n"
        << "    <frame_width>36</frame_width>\n"
        << "    <frame_height>48</frame_height>\n"
        << "    <position_x>50</position_x>\n"
        << "    <position_y>" << spawnY << "</position_y>\n"
        << "    <jump_force_y>-540</jump_force_y>\n"
        << "    <max_jump_height>90</max_jump_height>\n"
        << "    <layer>2</layer>\n"
        << "  </actor>\n"
        << "  <level_forces>\n"
        << "    <gravity_x>0</gravity_x>\n"
        << "    <gravity_y>400</gravity_y>\n"
        << "    <damping_x>0.7</damping_x>\n"
        << "    <damping_y>1.0</damping_y>\n"
        << "    <scrollSpeed>4</scrollSpeed>\n"
        << "  </level_forces>\n"
        << "  <goal>\n"
        << "    <type>" << goalType << "</type>\n"
        << "    <value>" << goalValue << "</value>\n"
        << "  </goal>\n"
        << "</level>\n";
    if (!xmlFile.commit()) {
        qWarning() << "EndlessLevelGenerator: failed to commit endless.xml";
        return QString();
    }

    // Level-Typ für UI (unter Level-Counter)
    QString levelDesc;
    if (doorType == 0) levelDesc = QStringLiteral("Normal");
    else if (doorType == 1) levelDesc = QStringLiteral("Schlüssel");
    else levelDesc = QString::number(goalValue) + QStringLiteral(" Münzen");
    setLastLevelType(levelDesc);

    qDebug() << "EndlessLevelGenerator: generated" << xmlPath;
    return QDir::cleanPath(QFileInfo(xmlPath).absoluteFilePath());
}
