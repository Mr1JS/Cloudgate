/*
 *  LevelForce.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "game/include/LevelForces.hpp"

namespace jumper
{

LevelForces::LevelForces()
    : m_gravity(0,400), m_damping(0.7, 1.0)
{

}

LevelForces::LevelForces(const Vector2f &gravity, const Vector2f &damping)
     : m_gravity(gravity),  m_damping(damping)
{

}

const Vector2f& LevelForces::gravity() const
{
    return m_gravity;
}

void LevelForces::setGravity(const Vector2f &gravity)
{
    m_gravity = gravity;
}

const Vector2f& jumper::LevelForces::damping() const
{
    return m_damping;
}

void jumper::LevelForces::setDamping(const Vector2f& damping)
{
    m_damping = damping;
}

}


