/*
 *  Level.hpp
 *
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef SRC_LEVEL_HPP_
#define SRC_LEVEL_HPP_

#include <string>
#include <SDL.h>

#include "MainWindow.hpp"
#include "Camera.hpp"
#include "TileSet.hpp"
#include "LevelForces.hpp"
#include "StateController.hpp"
#include "Monster.hpp"

#include <vector>

namespace jumper
{

class Physics;
class Actor;

/**
 * @brief Represents a level in the jumper game.
 */
class Level : public StaticRenderable
{
public:

    /**
     * Loads a level from the given file
     *
     * @param mainWindow	A pointer to a main window instance
     * @param filename		A file with a valid level definition
     */
    Level(MainWindow* mainWindow, std::string filename);

    /// Renders the level
    void render();

    /// Destructor
    ~Level();

    /// Updates the elements within the level according
    /// to the given SDL keyboard states
    void update(const Uint8* keystates);

    /// Returns the current level forces
    LevelForces& forces();

    /// Returns a pointer to the tile set representation
    TileSetRepresentation* tiles();

    const Camera& getCamera();
    
    /// Checks if actor is outside camera bounds (for game over)
    bool isActorOutsideCamera() const;

    StateController* getStateController();

    /// Adds a renderable on the given layer to the level.
    /// If the renderable is an actor, collisions will only
    /// be handled with tile sets representations on the
    /// same layer as the actor.
    void addRenderable(SDLRenderable* r, int layer = 0);

    /// Adds an actor that moves in the level and defines
    /// the camera movement. Renderable will be freed when
    /// the level is deleted.
    void addActor(Actor* actor, int layer = 0);

    /// Sets the tileset that is used for collision detection
    void addLevelTiles(TileSet* tiles, int layer = 0);

    /// Sets the current forces
    void setForces(const LevelForces& f);

private:

    /// Updates the actor accordint to the given keyboard states
    void updateActor(const Uint8* keystates);

    /// A SDL texture for the actor
    SDL_Texture*        	m_actorTexture;

    /// A pointer to the parent window
    MainWindow*         	m_mainWindow;

    /// The actor within the level
    Actor*                  m_actor;

    /// A camera that follows the actor
    Camera              	m_camera;

    /// A tile array
    TileSet*				m_tiles;

    /// The physics engine
    Physics*                m_physics;

    /// Curret forces in the level
    LevelForces             m_levelForce;

    /// A layer manager for correct rendering of the renderables
    LayerManager            m_layers;

    /// For managing level states such as hp, time elapsed and other things
    StateController*        m_stateController;

    /// Monster-Liste für Update (Renderables werden von LayerManager verwaltet)
    std::vector<Monster*>   m_monsters;

    void spawnMonsters();
    void spawnTraps();
};

} /* namespace jumper */

#endif /* SRC_LEVEL_HPP_ */
