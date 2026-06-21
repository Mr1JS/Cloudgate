/**
 * @file Physics.hpp
 * @brief Defines the Physics class for Box2D physics simulation and collision detection
 */

/*
 *  Physics.hpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 *
 *  Modified for Box2D-Integration.
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#include "TileSetRepresentation.hpp"
#include "ActorForces.hpp"
#include "Util.hpp"
#include "Vector.hpp"

#include <map>
#include <utility>
#include <vector>
#include <box2d/box2d.h>

namespace jumper
{

class Actor;
class Level;
class Physics;

/**
 * @brief Contact-Listener
 */
class ContactListener : public b2ContactListener
{
public:
    /// creates an instance of ContactListener, attached to given Player and Level
    ContactListener(Actor* actor, Level* level, Physics* physics);

    /// @brief called when player interacts with the hitbox of any other object in the world
    /// @param contact 
    void BeginContact(b2Contact* contact) override;

    /// @brief called when player contact with the hitbox of another object ends
    /// @param contact 
    void EndContact(b2Contact* contact) override;

private:

    /// the actor whose contact should be checked
    Actor*      m_actor;

    /// the level that is being played
    Level*      m_level;

    /// the physics instance to be used
    Physics*    m_physics;
};

/**
 * @brief Box2D-based physics engine.
 *        Handles collision and movement using Box2D.
 */
class Physics
{
public:
	/// Constructor with Actor and Level
    Physics(Actor* actor, Level* level);

    /// Destructor
    ~Physics();

    /// Executes one update step for physics
    void update();

    /// Called by ContactListener when colliding with a hazard
    void handleHazardContact(int tileId, const b2Vec2& tileCenter, const b2Vec2& actorCenter);

    /// Apply knockback (e.g. when colliding with a monster).
    /// @param otherCenter center in pixel coordinates.
    void applyKnockbackFromPosition(const Vector2f& otherCenter);

    /// returns Box2D body of actor
    b2Body* getActorBody() const;

    /// return time passed since last taking damage from a hazard
    unsigned int getLastHazardDamageTicks() const;

    /// true = player can take damage; false = still invincible
    bool canTakeDamage() const;

    /// Camera is only allowed to move after CAMERA_DELAY_MS time (e.g. 5s after level start)
    bool isCameraMovementEnabled() const;

    /// update time when damage was last taken (in ticks)
    void setLastHazardDamageTicks(unsigned int t);

    /// Movement lock after taking damage (ms)
    static constexpr unsigned int MOVEMENT_LOCK_MS = 250;

    /// Invincibility after taking damage (ms) - 2 seconds
    static constexpr unsigned int INVINCIBILITY_MS = 2000;

    // get Tile Data in ContactListener
    std::pair<std::string, std::string> getTileData(int tileId);

    /// Return Tile id (0-based) from RulesTiles.xml; -1 if not found
    int getTileIdByType(const std::string& type) const;

    /// Return all tile ids (0-based) of given type (e.g. "enemy" for monsters)
    std::vector<int> getTileIdsByType(const std::string& type) const;

    /// Return level width in tiles
    int getLevelWidth() const;

    /// Return level height in tiles
    int getLevelHeight() const;

    /// Queue Box2D body to be destroyed during next update step (e.g. coin after collection)
    void queueBodyForDestruction(b2Body* body);

    /// Queue creation of a body during next update step (e.g. after destroying a box and spawning an item)
    void queueCreateBodyForTile(int gx, int gy);

    /// Apply increased jump height by given factor (for jumping on a spring)
    void applySpringLaunch(float factor);

    /// Called by contact listener when interacting with a wall - prevents jittering when walking into it
    void addWallContact(float normalX);

    /// Called by ContactListener when wall contact ends
    void removeWallContact(float normalX);

private:

    /// Create static Box2D-Bodies from level tiles
    void buildLevelBodies();

    /// Create static body for tile at (gx, gy), depending on tileId at (gx, gy)
    void createBodyForTile(int gx, int gy);

    /// Convert pixel position to Box2D coordinates (meter, Y inverted)
    b2Vec2 toBox2D(const Vector2f& pixel) const;

    /// Convert Box2D position to pixels
    Vector2f fromBox2D(const b2Vec2& world) const;

    /// Apply player controls to Box2D body
    void applyPlayerInput(double dt);

    /// Check and correct camera bounds
    void enforceCameraBounds();

    /// Open closed doors when near door or coin target reached
    void updateDoors();

    /// Pointer to actor in level
    Actor*                              m_actor;

    /// TileSet for collision geometry
    TileSetRepresentation*              m_tiles;

    /// The level
    Level*                              m_level;

    /// Box2D World
    b2World*                            m_world;

    /// Box2D-Body of Actor
    b2Body*                             m_actorBody;

    /// Contact-Listener (Ownership)
    ContactListener*                    m_contactListener;

    /// Last Ticks value for delta time
    unsigned int                        m_lastTicks;

    /// Level start time (SDL_GetTicks), from which camera is allowed to move after CAMERA_DELAY_MS
    unsigned int                        m_cameraStartTicks;

    /// Time of last damage taken
    unsigned int                        m_lastHazardDamageTicks;

    /// Time until which plaer is invincible (SDL_GetTicks)
    unsigned int                        m_invincibleUntilTicks;

    /// Time (SDL_GetTicks) until which player is unable to move (after taking damage)
    unsigned int                        m_movementLockedUntilTicks;

    /// Left wall contact where player is not allowed to move past (prevents hanging/jittering)
    int                                 m_wallContactLeft;

    /// Right wall contact where player is not allowed to move past (prevents hanging/jittering)
    int                                 m_wallContactRight;

    /// Coyote-Time: Jump still possible after moving past ground (seconds)
    float                               m_coyoteTimeLeft;

    /// Scaling of pixels per meter for Box2D
    static constexpr float              PIXELS_PER_METER = 32.0f;

    /// Y-Offset of tile world in pixels
    static constexpr float              TILE_Y_OFFSET = 600.0f;

    /// Delay in ms, until camera starts moving (5 seconds)
    static constexpr unsigned int       CAMERA_DELAY_MS = 5000;

    /// Tile definitions (name, type, shape) from RulesTiles.xml
    std::map<int, TileInfo>             m_tileData;

    /// Bodies, to be destroyed in next update step (e.g. Collectibles after contact)
    std::vector<b2Body*>                m_bodiesToDestroy;

    /// (gx, gy) for bodies to be created in next update() step (e.g. after destroying random-box)
    std::vector<std::pair<int, int>>    m_tilesToCreateBodyFor;
};

} // namespace jumper

#endif // PHYSICS_H
