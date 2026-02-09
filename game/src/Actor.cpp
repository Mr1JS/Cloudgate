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

#include "game/include/Actor.hpp"
#include "game/include/MainWindow.hpp"

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
    m_blinking = false;
    m_superPotionActive = false;
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

        // Blinken: Super-Trank = bläulich, normale Invincibility = rötlich
        unsigned int ticks = SDL_GetTicks();
        bool isBlinkFrame = ((ticks / 100) % 2 == 0);
        
        if (m_superPotionActive)
        {
            // Super-Trank: Bläuliches Blinken (Color-Modulation)
            if (isBlinkFrame)
            {
                SDL_SetTextureColorMod(m_texture, 100, 150, 255);  // Bläulich
                SDL_SetTextureAlphaMod(m_texture, 255);
            }
            else
            {
                SDL_SetTextureColorMod(m_texture, 255, 255, 255);  // Normal
                SDL_SetTextureAlphaMod(m_texture, 255);
            }
        }
        else if (m_blinking)
        {
            // Normale Invincibility: Rötliches Blinken (nach Schaden)
            if (isBlinkFrame)
            {
                SDL_SetTextureColorMod(m_texture, 255, 150, 150);  // Rötlich
                SDL_SetTextureAlphaMod(m_texture, 255);
            }
            else
            {
                SDL_SetTextureColorMod(m_texture, 255, 255, 255);  // Normal
                SDL_SetTextureAlphaMod(m_texture, 255);
            }
        }
        else
        {
            // Kein Blinken: Normal
            SDL_SetTextureColorMod(m_texture, 255, 255, 255);
            SDL_SetTextureAlphaMod(m_texture, 255);
        }

        // Render current animation frame
        SDL_RenderCopyEx( m_mainWindow->renderer(), m_texture, &m_sourceRect, &target, 0, NULL, flip);
    }

}

void Actor::setBlinking(bool blink, bool superPotion)
{
    m_blinking = blink;
    m_superPotionActive = superPotion;
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

