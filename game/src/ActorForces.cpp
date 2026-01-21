/*
 *  ActorForce.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "include/ActorForces.hpp"

namespace jumper
{

ActorForces::ActorForces() :
        m_jumpForce(0, -440),
        m_moveForce(800, 0),
        m_maxJumpVelocity(400.0),
        m_maxRunVelocity(400.0),
        m_maxFallingVelocity(160),
        m_maxJumpHeight(150)
{

}

ActorForces::ActorForces(
        const Vector2f& moveForce,
        const Vector2f& jumpForce,
        double maxVel,
        double maxJumpVel,
        double maxJumpHeight,
        double maxFallVel
        )
: m_jumpForce(jumpForce), m_moveForce(moveForce),
  m_maxJumpVelocity(maxJumpVel), m_maxRunVelocity(maxVel), m_maxFallingVelocity(maxFallVel), m_maxJumpHeight(maxJumpHeight)
{

}

const Vector2f & ActorForces::jumpForce() const
{
    return m_jumpForce;
}

void ActorForces::setJumpForce(const Vector2f &jump)
{
    m_jumpForce = jump;
}

const Vector2f & ActorForces::moveForce() const
{
    return m_moveForce;
}

void ActorForces::setMoveForce(const Vector2f &move)
{
    m_moveForce = move;
}


double ActorForces::maxJumpVelocity() const
{
    return m_maxJumpVelocity;
}

void ActorForces::setMaxJumpVelocity(double maxVelJmp)
{
    maxVelJmp = m_maxJumpVelocity;
}

double ActorForces::maxRunVelocity() const
{
    return m_maxRunVelocity;
}

void ActorForces::setMaxRunVelocity(double maxVelRun)
{
    maxVelRun = m_maxRunVelocity;
}

double ActorForces::maxFallVelocity() const
{
    return m_maxFallingVelocity;
}

void ActorForces::setMaxFallVelocity(double maxVelFall)
{
    maxVelFall = m_maxFallingVelocity;
}

double ActorForces::maxJumpHeight() const
{
    return m_maxJumpHeight;
}

void ActorForces::setMaxJumpHeight(double jumpHeight)
{
    m_maxJumpHeight = jumpHeight;
}

}
