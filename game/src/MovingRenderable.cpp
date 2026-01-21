/*
 *  MovingRenderable.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "include/MovingRenderable.hpp"
#include "include/MainWindow.hpp"
#include "include/Util.hpp"

#include <cmath>
#include <iostream>
using namespace std;

namespace jumper
{

MovingRenderable::MovingRenderable(MainWindow* mainWindow, std::string filename)
    : SDLRenderable(mainWindow)
{
    // Load SDL texture from file
    m_texture = LoadTexture(m_mainWindow->renderer(), filename);
    computeSourceRect();

    // Init stuff
    m_rotation = 0;
}

MovingRenderable::MovingRenderable(
        MainWindow* mainWindow,
        SDL_Texture* texture,
        const Vector<double>& position,
        const double angle)
    : SDLRenderable(mainWindow, texture),
      m_rotation(angle)
{
    computeSourceRect();
}

void MovingRenderable::setWorldPosition(const Vector<double>& position)
{
    m_worldPosition = position;
    computeScreenCoordinates();
}

void MovingRenderable::computeScreenCoordinates()
{
    // Round world coordinates to nearest int
    m_targetRect.x = (int)floor(m_worldPosition.x());
    m_targetRect.y = (int)floor(m_worldPosition.y());

    // Copy screen coordinates to position vector
    m_position.setX(m_targetRect.x);
    m_position.setY(m_targetRect.y);
}

void MovingRenderable::operator*=(double angle)
{
    m_rotation += angle;

}

void MovingRenderable::operator+=(const Vector<double>& vec)
{
    m_worldPosition += vec;
    computeScreenCoordinates();
}

void MovingRenderable::operator-=(const Vector<double>& vec)
{
    m_worldPosition -= vec;
    computeScreenCoordinates();
}

Vector<double> MovingRenderable::worldPosition() const
{
    return m_worldPosition;
}

void MovingRenderable::setVelocity(const Vector<double>& velocity)
{
    m_velocity = velocity;
}

Vector<double>& MovingRenderable::velocity()
{
    return m_velocity;
}

void MovingRenderable::move()
{
    m_worldPosition += m_velocity;
    computeScreenCoordinates();
}

void MovingRenderable::render()
{
    if(readyToRender())
    {
        Vector<int> position = computeTargetPosition();

        // Just to be safe, copy target rect, don't modify	make
        SDL_Rect target;
        target.x = position.x();
        target.y = position.y();
        target.w = m_targetRect.w;
        target.h = m_targetRect.h;

        SDL_RenderCopyEx(m_mainWindow->renderer(), m_texture,
                         &m_sourceRect, &target, m_rotation,
                         NULL, SDL_FLIP_NONE);
    }
}


}
