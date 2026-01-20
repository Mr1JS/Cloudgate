/*
 *  Actor.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include <SDL.h>

#include "Actor.hpp"
#include "MainWindow.hpp"

namespace jumper
{

Actor::Actor(MainWindow* mainWindow, std::string filename)
    : AnimatedRenderable(mainWindow, filename)
{
    m_jumping = 0;
    m_wantsToJump = false;
    m_onGround = false;
    m_jumpStart = 0;
    setWorldPosition(Vector2f(100, 0));
}

Actor::Actor(MainWindow* mainWindow, SDL_Texture* texture, int frameWidth, int frameHeight, int numFrames)
    : AnimatedRenderable(mainWindow, texture, frameWidth, frameHeight, numFrames)
{
    m_jumping = 0;
    m_wantsToJump = false;
    m_onGround = false;
    m_jumpStart = 0;
    setWorldPosition(Vector2f(100, 0));
}

void Actor::setPhysics(ActorForces p)
{
    m_forces = p;
}

Actor::~Actor()
{

}

void Actor::setWantsToJump(bool j)
{
    m_wantsToJump = j;
}

bool Actor::wantsToJump()
{
    return m_wantsToJump;
}


void Actor::render()
{
    nextFrame();
    SDL_Rect target;

    Vector<int> position = computeTargetPosition();

    target.x = position.x();
    target.y = position.y();
    target.w = m_frameWidth;
    target.h = m_frameHeight;

    // Do not render if actor is outside frustrum
    //if(target.x + target.h > 0 && target.x + target.h < m_camera.w())
    {


        SDL_RendererFlip flip;
        if(m_velocity.x() > 0)
        {
            flip = SDL_FLIP_HORIZONTAL;
        }
        else
        {
            flip = SDL_FLIP_NONE;
        }

        // Render current animation frame
        SDL_RenderCopyEx( m_mainWindow->renderer(), m_texture, &m_sourceRect, &target, 0, NULL, flip);
    }

}

bool Actor::onGround() const
{
    return m_onGround;
}

void Actor::setOnGround(bool onGround)
{
    m_onGround = onGround;
}

ActorForces& Actor::forces()
{
    return m_forces;
}

bool Actor::jumping()
{
    return m_jumping;
}

void Actor::setJumping(bool jump)
{
    if(jump)
    {
        m_jumpStart = m_position.y();
    }
    m_jumping = jump;
}

int Actor::jumpStart()
{
    return m_jumpStart;
}


void Actor::getCollision(Actor& other)
{

}



} /* namespace jumper */

