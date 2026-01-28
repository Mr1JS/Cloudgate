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

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

#include <SDL_image.h>

namespace jumper
{


Level::Level(MainWindow* mainWindow, std::string filename)
    : StaticRenderable(mainWindow),
      m_mainWindow(mainWindow),
      m_camera(400, 1000, mainWindow->w(), mainWindow->h()),  // Kamera weiter rechts und weiter unten
      m_layers(&m_camera)
{
    m_physics   = 0;
    m_actor     = 0;
    m_tiles     = 0;

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

    m_stateController = new StateController(mainWindow, filename);

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
    if (m_stateController->isPaused()) {
        dt = 0;
    }
    m_camera.update(dt);
    
    if(m_physics)
    {
        // Update actor according to given key states
        updateActor(keystates);

        // Run physics
        m_physics->update();
    }
    
    if (m_stateController != nullptr)
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
