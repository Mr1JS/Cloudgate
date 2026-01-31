/*
 *  Level.cpp
 *
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "game/include/Level.hpp"
#include "game/include/MainWindow.hpp"
#include "game/include/Actor.hpp"
#include "game/include/Util.hpp"
#include "game/include/Physics.hpp"
#include "game/include/LevelParser.hpp"
#include "game/include/Monster.hpp"

#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <cmath>

using std::cout;
using std::endl;

#include <SDL_image.h>

namespace jumper
{


Level::Level(MainWindow* mainWindow, std::string filename)
    : StaticRenderable(mainWindow),
      m_mainWindow(mainWindow),
      m_camera(322, 1000, mainWindow->w(), mainWindow->h()),  // Kamera weiter rechts und weiter unten
      m_layers(&m_camera)
{
    m_physics   = 0;
    m_actor     = 0;
    m_tiles     = 0;

    m_stateController = new StateController(mainWindow, filename);

    // Setup level attributes from config file
    LevelParser p(filename, this, m_mainWindow);

    // Füge Wand-Tiles links und rechts hinzu (als sichtbare Grenzen)
    if(m_tiles && m_tiles->tiles())
    {
        TileSetRepresentation* tileRep = m_tiles->tiles();
        int tileWidth = tileRep->tileWidth();
        int levelHeight = tileRep->height();
        int levelWidth = tileRep->width();
        
        // Berechne die Kamera-Grenzen in Tile-Koordinaten
        int cameraX = m_camera.x();  // Kamera X in World-Koordinaten (400)
        int cameraWidth = m_camera.width();  // Kamera Breite (800)
        int cameraRight = cameraX + cameraWidth;  // Rechte Grenze in World-Koordinaten (1200)
        
        // Konvertiere World-Koordinaten zu Tile-Koordinaten
        // Linke Wand: Bei Tile 0 (linker Rand des Levels)
        int leftTileX = 0;
        // Rechte Wand: Bei Kamera X + Breite (rechter Rand der Kamera)
        int rightTileX = cameraRight / tileWidth;
        
        // Stelle sicher, dass die rechte Wand innerhalb der Level-Breite ist
        if(rightTileX >= levelWidth) rightTileX = levelWidth - 1;
        
        // Verwende Tile-ID 1 für die Wände (normalerweise ist 1 ein solides Tile)
        int wallTileId = 1;
        
        // Füge linke Wand hinzu (von unten nach oben)
        if(leftTileX >= 0 && leftTileX < levelWidth)
        {
            for(int y = 0; y < levelHeight; y++)
            {
                tileRep->insert(leftTileX, y, wallTileId);
            }
        }
        
        // Füge rechte Wand hinzu (von unten nach oben)
        if(rightTileX >= 0 && rightTileX < levelWidth)
        {
            for(int y = 0; y < levelHeight; y++)
            {
                tileRep->insert(rightTileX, y, wallTileId);
            }
        }
    }

    spawnTraps();

//    m_actor = new Actor(mainWindow, "../res/actor.spr");
//     m_actor->setFPS(10);
//     m_actor->setWorldPosition(Vector<double>(800, 100));

//      m_layers.addRenderable(m_actor, 4);
    if(m_actor)
    {
        m_camera.setFocus(m_actor);
        m_physics = new Physics(m_actor, this);
    }

    spawnMonsters();
}

void Level::spawnTraps()
{
    if(!m_tiles || !m_tiles->tiles()) return;

    TileSetRepresentation* tileRep = m_tiles->tiles();
    int levelW = tileRep->width();
    int levelH = tileRep->height();

    std::vector<std::pair<int, int>> trapPositions;
    for(int gy = 1; gy < levelH; ++gy)
    {
        for(int gx = 1; gx < levelW - 1; ++gx)
        {
            int below = tileRep->get(gx, gy);
            int at = tileRep->get(gx, gy - 1);
            if(below > 0 && at <= 0)
            {
                trapPositions.push_back({gx, gy - 1});
            }
        }
    }

    if(trapPositions.empty()) return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(trapPositions.begin(), trapPositions.end(), gen);

    const int maxTraps = 2;
    const int minDistance = 64;
    std::vector<std::pair<int, int>> placed;
    int count = 0;
    const int spikeId = 128;  // Spikes (1-basiert)
    for(size_t i = 0; i < trapPositions.size() && count < maxTraps; ++i)
    {
        int gx = trapPositions[i].first, gy = trapPositions[i].second;
        bool tooClose = false;
        for(const auto& p : placed)
        {
            int dx = gx - p.first, dy = gy - p.second;
            if(std::abs(dx) * 32 + std::abs(dy) * 32 < minDistance) { tooClose = true; break; }
        }
        if(tooClose) continue;
        tileRep->insert(gx, gy, spikeId);
        placed.push_back({gx, gy});
        count++;
    }
}

void Level::spawnMonsters()
{
    if(!m_tiles || !m_tiles->tiles() || !m_tiles->texture()) return;

    TileSetRepresentation* tileRep = m_tiles->tiles();
    int tw = m_tiles->tileWidth();
    int th = m_tiles->tileHeight();
    int levelW = tileRep->width();
    int levelH = tileRep->height();
    const int TILE_Y_OFFSET = 600;

    struct SpawnPoint { double x, y, leftBound, rightBound; };
    std::vector<SpawnPoint> spawnPositions;
    const int monsterW = 32;
    for(int gy = 1; gy < levelH; ++gy)
    {
        for(int gx = 1; gx < levelW - 1; ++gx)
        {
            int below = tileRep->get(gx, gy);
            int at = tileRep->get(gx, gy - 1);
            if(below > 0 && at <= 0)
            {
                int gxLeft = gx, gxRight = gx;
                while(gxLeft > 0 && tileRep->get(gxLeft - 1, gy) > 0) gxLeft--;
                while(gxRight < levelW - 1 && tileRep->get(gxRight + 1, gy) > 0) gxRight++;
                double leftBound = gxLeft * tw;
                double rightBound = (gxRight + 1) * tw - monsterW;
                if(rightBound - leftBound >= monsterW)
                {
                    double wx = gx * tw + tw / 2.0 - monsterW / 2.0;
                    double wy = gy * th + TILE_Y_OFFSET - 64;
                    spawnPositions.push_back({wx, wy, leftBound, rightBound});
                }
            }
        }
    }

    if(spawnPositions.empty()) return;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(spawnPositions.begin(), spawnPositions.end(), gen);

    const int maxGhosts = 2;
    const int maxSnakes = 2;
    const double minDistance = 96;  // Mindestabstand zwischen Monstern (3 Tiles)
    std::vector<std::pair<double, double>> placedPositions;
    int ghostCount = 0, snakeCount = 0;
    for(size_t i = 0; i < spawnPositions.size() && (ghostCount < maxGhosts || snakeCount < maxSnakes); ++i)
    {
        const auto& sp = spawnPositions[i];
        bool tooClose = false;
        for(const auto& placed : placedPositions)
        {
            double dx = sp.x - placed.first;
            double dy = sp.y - placed.second;
            if(std::sqrt(dx*dx + dy*dy) < minDistance)
            {
                tooClose = true;
                break;
            }
        }
        if(tooClose) continue;
        Monster::Type type;
        if(ghostCount >= maxGhosts) type = Monster::Type::Snake;
        else if(snakeCount >= maxSnakes) type = Monster::Type::Ghost;
        else type = (gen() % 2 == 0) ? Monster::Type::Ghost : Monster::Type::Snake;
        if((type == Monster::Type::Ghost && ghostCount >= maxGhosts) ||
           (type == Monster::Type::Snake && snakeCount >= maxSnakes))
            continue;
        Monster* m = new Monster(m_mainWindow, m_tiles->texture(), type, sp.x, sp.y,
                                sp.leftBound, sp.rightBound,
                                tw, th, m_tiles->tilesPerRow(), m_tiles->tileOffset());
        m_monsters.push_back(m);
        m_layers.addRenderable(m, 2);
        placedPositions.push_back({sp.x, sp.y});
        if(type == Monster::Type::Ghost) ghostCount++; else snakeCount++;
    }
}

void Level::setForces(const LevelForces &f)
{
    m_levelForce = f;
}

void Level::addRenderable(SDLRenderable*  r, int layer)
{
    if(r)
    {
        m_layers.addRenderable(r, layer);
    }
}

void Level::addActor(Actor *actor, int layer)
{
    if(actor)
    {
        m_actor = actor;
        m_camera.setFocus(m_actor);
        m_layers.addRenderable(actor, layer);
    }
}

void Level::addLevelTiles(TileSet *tiles, int layer)
{
    if(tiles)
    {
        m_tiles = tiles;
        m_layers.addRenderable(tiles, layer);
    }
}

StateController* Level::getStateController()
{
    return m_stateController;
}

void Level::update(const Uint8* keystates)
{
    // Update camera (automatisches Scrollen nach oben)
    // Berechne delta time (vereinfacht: 1/60 Sekunden bei 60 FPS)
    double dt = 1.0 / 60.0;
    if (m_stateController->isPaused())
    {
        dt = 0;
    }

    m_camera.update(dt);
    
    if(m_physics)
    {
        // Update actor according to given key states
        updateActor(keystates);
        
        // Run physics
        m_physics->update();

        // Monster-Kollision: Schaden + Knockback
        unsigned int now = SDL_GetTicks();
        const unsigned int damageCooldownMs = 1000;
        if(m_actor && now - m_physics->getLastHazardDamageTicks() >= damageCooldownMs)
        {
            double ax = m_actor->worldPosition().x(), ay = m_actor->worldPosition().y();
            int aw = m_actor->w(), ah = m_actor->h();
            for(Monster* mon : m_monsters)
            {
                double mx = mon->worldPosition().x(), my = mon->worldPosition().y();
                int mw = mon->w(), mh = mon->h();
                if(ax < mx + mw && ax + aw > mx && ay < my + mh && ay + ah > my)
                {
                    m_physics->setLastHazardDamageTicks(now);
                    if(m_stateController) m_stateController->decrementHp(1);
                    double monCx = mx + mw / 2.0, monCy = my + mh / 2.0;
                    m_physics->applyKnockbackFromPosition(Vector2f(monCx, monCy));
                    break;
                }
            }
        }
    }

    for(Monster* m : m_monsters)
        m->update(dt);
        
    if (m_stateController)
    {
        m_stateController->updateGameTime();
        
        if (m_stateController->isPaused() && (keystates[SDL_SCANCODE_LEFT ] || keystates[SDL_SCANCODE_RIGHT]
            || keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_SPACE])) {
                m_stateController->startGameTime();
        }
    }
}

TileSetRepresentation* Level::tiles()
{
    if(m_tiles)
    {
        return m_tiles->tiles();
    }
    return nullptr;
}

void Level::updateActor(const Uint8* keystates)
{
    if(m_actor)
    {
        m_actor->forces().setMoveForce(Vector2f(0.0, 0.0));

        if(keystates[SDL_SCANCODE_LEFT ])
        {
            m_actor->forces().setMoveForce(Vector2f(-100, 0.0));
        }
        if(keystates[SDL_SCANCODE_RIGHT])
        {
            m_actor->forces().setMoveForce(Vector2f(100.0, 0.0));
        }

        if(keystates[SDL_SCANCODE_A])
        {
            (*m_actor) *= -1;
        }

        if(keystates[SDL_SCANCODE_D])
        {
            (*m_actor) *= 1;
        }
        if(keystates[SDL_SCANCODE_SPACE])
        {
            m_actor->setWantsToJump(true);
        }
    }
}

LevelForces& Level::forces()
{
    return m_levelForce;
}

void Level::render()
{
    m_layers.render();
}

const Camera& Level::getCamera()
{
    return m_camera;
}

bool Level::isActorOutsideCamera() const
{
    if(!m_actor)
    {
        return false;
    }
    
    const Camera& camera = m_camera;
    Vector<double> actorPos = m_actor->worldPosition();
    int actorHeight = m_actor->h();
    
    // Kamera-Grenzen in World-Koordinaten
    int cameraY = camera.y();
    int cameraHeight = camera.height();
    int cameraBottom = cameraY + cameraHeight;
    
    // Prüfe, ob der untere Rand des Spielers den unteren Rand der Kamera berührt oder überschreitet
    // In SDL-Koordinaten: Y=0 ist oben, größere Y-Werte sind weiter unten
    // actorPos.y() ist die obere Kante des Spielers
    // actorPos.y() + actorHeight ist die untere Kante des Spielers
    // cameraBottom ist die untere Kante der Kamera
    
    // Game Over: Wenn die untere Kante des Spielers sich dem unteren Rand der Kamera nähert
    // Da die Kamera nach oben scrollt, wird der Spieler relativ zur Kamera nach unten "fallen"
    double actorBottom = actorPos.y() + actorHeight;
    
    // Prüfe, ob der Spieler sich dem unteren Rand der Kamera nähert
    // WICHTIG: Wir lösen Game Over aus, wenn der Spieler die HÄLFTE der Kamera-Höhe erreicht
    // Das bedeutet: Game Over, wenn der Spieler in der unteren Hälfte der Kamera ist
    // Dies sorgt dafür, dass Game Over sehr früh auslöst
    int gameOverThreshold = cameraY + (cameraHeight / 2);  // Mitte der Kamera
    
    if(actorBottom >= gameOverThreshold)
    {
        return true;  // Unterer Rand des Spielers berührt oder überschreitet unteren Rand der Kamera
    }
    
    return false;  // Spieler ist noch innerhalb des Kamera-Bereichs
}

Level::~Level()
{
    if(m_physics)
    {
        delete m_physics;
    }


}


} /* namespace jumper */
