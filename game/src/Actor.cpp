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
    m_saltoRotation = 0.0;
    m_lastSaltoTicks = 0;
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
    m_breakTilesModeActive = false;
    m_saltoRotation = 0.0;
    m_lastSaltoTicks = 0;
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

        // Blinken: Super-Trank = bläulich, Green-Potion = grün, Invincibility = rötlich
        unsigned int ticks = SDL_GetTicks();
        bool isBlinkFrame = ((ticks / 100) % 2 == 0);
        
        if (m_superPotionActive)
        {
            if (isBlinkFrame)
            {
                SDL_SetTextureColorMod(m_texture, 100, 150, 255);  // Bläulich
                SDL_SetTextureAlphaMod(m_texture, 255);
            }
            else
            {
                SDL_SetTextureColorMod(m_texture, 255, 255, 255);
                SDL_SetTextureAlphaMod(m_texture, 255);
            }
        }
        else if (m_breakTilesModeActive)
        {
            if (isBlinkFrame)
            {
                SDL_SetTextureColorMod(m_texture, 100, 255, 150);  // Grünlich
                SDL_SetTextureAlphaMod(m_texture, 255);
            }
            else
            {
                SDL_SetTextureColorMod(m_texture, 255, 255, 255);
                SDL_SetTextureAlphaMod(m_texture, 255);
            }
        }
        else if (m_blinking)
        {
            if (isBlinkFrame)
            {
                SDL_SetTextureColorMod(m_texture, 255, 150, 150);  // Rötlich
                SDL_SetTextureAlphaMod(m_texture, 255);
            }
            else
            {
                SDL_SetTextureColorMod(m_texture, 255, 255, 255);
                SDL_SetTextureAlphaMod(m_texture, 255);
            }
        }
        else
        {
            SDL_SetTextureColorMod(m_texture, 255, 255, 255);
            SDL_SetTextureAlphaMod(m_texture, 255);
        }

        // Salto bei Super-Trank: in der Luft rotieren
        unsigned int now = SDL_GetTicks();
        if (m_superPotionActive && !m_onGround)
        {
            if (m_lastSaltoTicks != 0)
                m_saltoRotation += (now - m_lastSaltoTicks) * 0.72;  // ~1 Salto in ~500 ms
            if (m_saltoRotation >= 360.0) m_saltoRotation -= 360.0;
            if (m_saltoRotation < 0.0) m_saltoRotation += 360.0;
        }
        else
            m_saltoRotation = 0.0;
        m_lastSaltoTicks = now;

        double angle = m_saltoRotation;
        if (flip == SDL_FLIP_HORIZONTAL)
            angle = -angle;

        // Render current animation frame (mit Salto-Rotation bei Super-Trank)
        SDL_RenderCopyEx(m_mainWindow->renderer(), m_texture, &m_sourceRect, &target, angle, NULL, flip);
    }

}

void Actor::setBlinking(bool blink, bool superPotion, bool breakTilesMode)
{
    m_blinking = blink;
    m_superPotionActive = superPotion;
    m_breakTilesModeActive = breakTilesMode;
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

