/*
 *  ActorForce.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef PLAYER_PROPERTIES_H
#define PLAYER_PROPERTIES_H

#include "Vector.hpp"

namespace jumper
{

/**
 * A class to represent the physical properties of
 * a player.
 */
class ActorForces
{
public:

    /**
     * Constructor.
     */
    ActorForces();

    /***
     * Constructor
     *
     * @param position		Initial position
     * @param moveForce		Initial move force
     * @param velocity		Initial velocity
     * @param maxVel		Maximum velocity
     * @param maxJumpVel	Maximum jump velocity
     * @param maxJumpHeight Maximum jump height
     * @param maxRunVel		Maximum run velocity
     * @param maxFallVel	Maximum fall velocity
     */
    ActorForces(
            const Vector2f& moveForce,
            const Vector2f& jumpForce,
            double maxVel,
            double maxJumpVel,
            double maxJumpHeight,
            double maxFallVel
            );

    /***
     * Gets the force for jumping
     */
    const Vector2f& jumpForce() const;

    /***
     * Sets the force for jumping
     */
    void setJumpForce(const Vector2f &jump);

    /**
     * Returns the move force to push the player
     */
    const Vector2f& moveForce() const;

    /**
     * Sets the move force to push the player
     */
    void setMoveForce(const Vector2f &move);

    /***
     * Returns the maximum velocity when jumping
     */
    double maxJumpVelocity() const;

    /***
     * Sets the maximum velocity when jumping
     */
    void setMaxJumpVelocity(double maxVelJmp);

    /***
     * Returns the maximum running velocity
     */
    double maxRunVelocity() const;

    /**
     * Sets the maximum running velocity
     */
    void setMaxRunVelocity(double maxVelRun);

    /***
     * Returns the maximum falling velocity
     */
    double maxFallVelocity() const;

    /***
     * Sets the maximum falling velocity
     */
    void setMaxFallVelocity(double maxVelFall);

    /**
     * Returns the maximum jump height
     */
    double maxJumpHeight() const;

    /**
     * Sets the maximum jump height.
     *
     * @param jumpHeight
     */
    void setMaxJumpHeight(double jumpHeight);


private:

    /// Jump force
    Vector2f    m_jumpForce;

    /// Force that pushes the player
    Vector2f    m_moveForce;

    /// Maximum jump velocity
    double      m_maxJumpVelocity;

    /// Maximum run velocity
    double      m_maxRunVelocity;

    /// Maxumin fall velocity
    double      m_maxFallingVelocity;

    /// Maximum jump height
    double      m_maxJumpHeight;
};

}

#endif //JUMPER_PHYSICPLAYER_H
