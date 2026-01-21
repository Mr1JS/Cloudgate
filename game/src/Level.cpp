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

#include "include/Level.hpp"
#include "include/MainWindow.hpp"
#include "include/Actor.hpp"
#include "include/Util.hpp"
#include "include/Physics.hpp"
#include "include/LevelParser.hpp"

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
      m_camera(400, 600, mainWindow->w(), mainWindow->h()),  // Kamera weiter rechts und unten
      m_layers(&m_camera)
{
    m_physics   = 0;
    m_actor     = 0;
    m_tiles     = 0;

    // Setup level attributes from config file
    LevelParser p(filename, this, m_mainWindow);

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

void Level::update(const Uint8* keystates)
{
    if(m_physics)
    {
        // Update actor according to given key states
        updateActor(keystates);

        // Run physics
        m_physics->update();
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

Level::~Level()
{
    if(m_physics)
    {
        delete m_physics;
    }


}


} /* namespace jumper */
