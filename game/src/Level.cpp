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
#include <cmath>

using std::cout;
using std::endl;

#include <SDL_image.h>

namespace jumper
{

/// @param mainWindow 
/// @param filename 
Level::Level(MainWindow* mainWindow, std::string filename)
    : StaticRenderable(mainWindow),
      m_mainWindow(mainWindow),
      m_camera(322, 1000, mainWindow->w(), mainWindow->h()),  // Kamera weiter rechts und weiter unten
      m_layers(&m_camera)
{
    m_physics         = 0;
    m_actor           = 0;
    m_tiles           = 0;
    m_goalType        = GOAL_NONE;
    m_goalTargetValue = 0;
    m_goalState       = GOALSTATE_NONE;
    m_coin_count      = 0;

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
        int wallTileId = 85;
        
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

    spawnMonsters();  // Vor Physics: Monster-Tiles aus Grid entfernen, damit keine Box2D-Bodies entstehen

//    m_actor = new Actor(mainWindow, "../res/actor.spr");
//     m_actor->setFPS(10);
//     m_actor->setWorldPosition(Vector<double>(800, 100));

//      m_layers.addRenderable(m_actor, 4);
    if(m_actor)
    {
        m_camera.setFocus(m_actor);
        m_physics = new Physics(m_actor, this);
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

    // Tile-IDs: 1-basiert (145/146, 147/148) oder 0-basiert (144/145, 146/147)
    const int GHOST_TOP_A = 145, GHOST_BOTTOM_A = 146;  // 1-basiert
    const int GHOST_TOP_B = 144, GHOST_BOTTOM_B = 145;  // 0-basiert
    const int SNAKE_TOP_A = 147, SNAKE_BOTTOM_A = 148;
    const int SNAKE_TOP_B = 146, SNAKE_BOTTOM_B = 147;
    const int monsterW = 32;

    for(int gy = 0; gy < levelH - 1; ++gy)
    {
        for(int gx = 0; gx < levelW; ++gx)
        {
            int topTile = tileRep->get(gx, gy);
            int botTile = tileRep->get(gx, gy + 1);
            Monster::Type type;
            bool isMonster = false;
            if((topTile == GHOST_TOP_A && botTile == GHOST_BOTTOM_A) ||
               (topTile == GHOST_TOP_B && botTile == GHOST_BOTTOM_B))
            {
                type = Monster::Type::Ghost;
                isMonster = true;
            }
            else if((topTile == SNAKE_TOP_A && botTile == SNAKE_BOTTOM_A) ||
                    (topTile == SNAKE_TOP_B && botTile == SNAKE_BOTTOM_B))
            {
                type = Monster::Type::Snake;
                isMonster = true;
            }
            if(!isMonster) continue;

            double wx = gx * tw;
            double wy = gy * th + TILE_Y_OFFSET;

            int platformRow = gy + 2;
            int gxLeft = gx, gxRight = gx;
            if(platformRow < levelH)
            {
                while(gxLeft > 0 && tileRep->get(gxLeft - 1, platformRow) > 0) gxLeft--;
                while(gxRight < levelW - 1 && tileRep->get(gxRight + 1, platformRow) > 0) gxRight++;
            }
            double leftBound = gxLeft * tw;
            double rightBound = (gxRight + 1) * tw - monsterW;
            if(rightBound - leftBound < monsterW)
                rightBound = leftBound + monsterW;

            tileRep->insert(gx, gy, 0);
            tileRep->insert(gx, gy + 1, 0);

            Monster* m = new Monster(m_mainWindow, m_tiles->texture(), type, wx, wy,
                                    leftBound, rightBound,
                                    tw, th, m_tiles->tilesPerRow(), m_tiles->tileOffset());
            m_monsters.push_back(m);
            m_layers.addRenderable(m, 2);
        }
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

void Level::setResPath(std::string path)
{
    m_resPath = path;
}

std::string Level::getResPath()
{
    return m_resPath;
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
    if (!m_stateController->isPaused())
    {
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
        {
            m->update(dt, m_actor);
        }
    }
        
    if (m_stateController)
    {
        // advance game timer - except if game is paused
        m_stateController->updateGameTime();

        if (m_goalType == GOAL_TIME)
        {
            checkAndUpdateGoalState();
        }
        
        // unpause game if any key is pressed
        if (m_stateController->isPaused() && (keystates[SDL_SCANCODE_LEFT ] || keystates[SDL_SCANCODE_RIGHT]
            || keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_SPACE]
            || keystates[SDL_SCANCODE_P]))
        {
                m_stateController->startGame();
        }

        if (keystates[SDL_SCANCODE_P])
        {
            std::cout << "Pausing game" << std::endl;
            m_stateController->stop();
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

bool Level::isGameOver() const
{
    // Check if HP is 0 or below
    if(m_stateController)
    {
        int hp = m_stateController->getHp();
        if(hp <= 0)
        {
            return true;
        }
    }
    
    // Check if actor fell out of camera
    return isActorOutsideCamera() || m_goalState == GOALSTATE_GAME_OVER;
}

GoalState Level::checkAndUpdateGoalState() {
    // if we already won or lost, there's no need to check for anything anmore
    switch (m_goalState)
    {
    case GOALSTATE_GAME_OVER:
    case GOALSTATE_LEVEL_FINISHED:
        return m_goalState;
    }

    GoalState state = m_goalState;
    switch (m_goalType)
    {
    case GOAL_TIME:
        if (m_stateController->getRuntime() > m_goalTargetValue*1000)
        {
            state = GOALSTATE_GAME_OVER;
        }
        else
        {
            state = GOALSTATE_WINNABLE;
        }
        break;

    case GOAL_COINS:
        if (m_coin_count >= m_goalTargetValue)
        {
            state = GOALSTATE_WINNABLE;
        }
        else
        {
            state = GOALSTATE_NONE;
        }
        break;
        
    default:
        state = GOALSTATE_WINNABLE;
        break;
    }

    m_goalState = state;

    return m_goalState;
}

void Level::win()
{
    m_goalState = GOALSTATE_LEVEL_FINISHED;
}

bool Level::isLevelFinished()
{
    bool finished = m_goalState == GOALSTATE_LEVEL_FINISHED;
//    std::cout << "Level is finished? " << finished << std::endl;
    return finished;
}

void Level::setGoalCondition(int type, int targetValue)
{
    m_goalType = GoalType(type);
    m_goalTargetValue = targetValue;
}

void Level::setCameraSettings(float scrollSpeed, float pos_y)
{
    m_camera.setScrollSpeed(scrollSpeed);
    m_camera.setY(pos_y);
}

void Level::collect(int tileId)
{
    // get tile at actor x / y
    TileSetRepresentation* tileRep = tiles();
    int idx = tileRep->get(m_actor->x(), m_actor->y());
    std::cout << "TileID: " << tileId << ", " << idx << std::endl;
    
    //m_layers->removeRenderable();
}

Level::~Level()
{
    if(m_physics)
    {
        delete m_physics;
    }

    if (m_stateController)
    {
        delete m_stateController;
    }
}


} /* namespace jumper */
