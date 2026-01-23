/*
 *  Physics.hpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#include "TileSetRepresentation.hpp"
#include "ActorForces.hpp"

namespace jumper {

class Actor;
class Level;

/**
 * @brief Computes the collisions between a level
 *        and an actor instance using the given actor and level
 *        force parameters.
 */
class Physics
{
public:
	/// Constructor using an Actor that interacts with an Level.
    Physics(Actor* actor, Level* level);

    /// Performs all computations
    void update();

private:

    /// Resolves all collisions between actor and level tiles
    void resolveCollision();

    /// Computes the new desired position of an actor according
    /// to the internal physics model
    void updateActorPosition(double dt);

    /// Gets the tiles surrounding the actor at the given position.
    /// Tiles will contain the tile indices of the 8 surrounding tiles.
    /// Memory has to be pre-allocated.
    void getSurroundingTiles(const Vector2f& position, Vector<int>* tiles);

    /// The actor within the level
    Actor*                  m_actor;

    /// The used tile set representation
    TileSetRepresentation*  m_tiles;

    /// The level
    Level*                  m_level;

    /// Clock time SDL init. Used to compute the time
    /// that elapsed since the last update() call.
    unsigned int            m_lastTicks;
};

} // namespace jumper

#endif // PHYSICS_H
