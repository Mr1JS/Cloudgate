/**
 * @file Level.hpp
 * @brief Defines the Level class which represents a complete game level with actors, monsters and physics
 */

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

/***
 * Type of goal condition. \\
 * NONE  = no specific condition, just reach the goal. \\
 * COINS = reach the goal with a certain number of coins. \\
 * TIME  = reach the goal within a certain number of seconds.
 */
enum GoalType
{
    GOAL_NONE = 0,
    GOAL_COINS,
    GOAL_TIME
};

/**
 * Represents the level's clear status. \\
 * GOALSTATE_NONE           = goal condition not met. \\
 * GOALSTATE_GAME_OVER      = goal condition failed. \\
 * GOALSTATE_WINNABLE       = goal condition met, but not yet done.
 * GOALSTATE_LEVEL_FINISHED = finished the level. \\
 */
enum GoalState
{
    GOALSTATE_NONE,
    GOALSTATE_GAME_OVER,
    GOALSTATE_WINNABLE,
    GOALSTATE_LEVEL_FINISHED
};

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

    /// Checks if game is over (HP <= 0)
    bool isGameOver() const;

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

    /// checks and updates the GoalState depending on current conditions, then returns it
    GoalState checkAndUpdateGoalState();

    /// set goalState to GOALSTATE_LEVEL_FINISHED
    void win();

    /// whether player has reached the exit and level should be exited.
    /// Returns true when goalState is GOALSTATE_LEVEL_FINISHED
    bool isLevelFinished();

    /// @brief configure the type of goal for the level
    /// @param type instance of \ref GoalType
    /// @param targetValue value to meet for goal type, if necessary
    void setGoalCondition(int type, int targetValue);

    /// Aktueller Zielmodus (z.B. GOAL_COINS)
    GoalType goalType() const { return m_goalType; }

    /// set res path to access RulesTiles.xml
    void setResPath(std::string path);
    
    /// get res path
    std::string getResPath();

    /// set scroll speed in camera
    void setCameraSettings(float scrollSpeed, float pos_y);

    /// removes Tile at (gx, gy) coordinates from map (e.g. a collected coin)
    void removeTileAt(int gx, int gy);

    /// Returns tile id at (gx, gy), 0 = empty, otherwise tileId from RulesTiles.xml +1
    int getTileAt(int gx, int gy) const;

    /// Set Tile at (gx, gy), value = 0 empty, otherwise tileId from RulesTiles.xml +1
    void setTileAt(int gx, int gy, int value);

    /// Spawns a monster (Ghost or Snake) at (gx, gy). gy = ground row; top tile: gy=-1.
    /// Uses the same monster ai as spawnMonsters() (Snake: move left-right, Ghost: 5s pursuit).
    void spawnMonsterAt(int gx, int gy, Monster::Type type);

private:

    /// Updates the actor accordint to the given keyboard states
    void updateActor(const Uint8* keystates);

    /// Spawn monsters, as defined in level definition
    void spawnMonsters();

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

    /// Monster-Liste for update() (Renderables are managed by LayerManager)
    std::vector<Monster*>   m_monsters;

    /// Type of goal requirement, e.g. time, coins, etc.
    GoalType                m_goalType;

    /// Value to reach the target, if necessary. E.g. coin count, time limit in sec, etc.
    int                     m_goalTargetValue;

    /// Whether player has met the goal requirements, or not, or failed, or finished the level
    GoalState               m_goalState;

    /// set res path to access RulesTiles.xml
    std::string             m_resPath = "";

    /// Whether spacebar was pressed in previous frame (for not setting wantsToJump twice)
    bool                    m_prevSpacePressed = false;
};

} /* namespace jumper */

#endif /* SRC_LEVEL_HPP_ */
