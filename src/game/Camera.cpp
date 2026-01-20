/*
 *  Camera.cpp
 *
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "Camera.hpp"

#include <iostream>
using namespace std;

namespace jumper
{

Camera::Camera(int x, int y, int w, int h)
    : m_position(x,y)
{
    m_actor = 0;
    m_height = h;
    m_width = w;
}

void Camera::setFocus(MovingRenderable* a)
{
    m_actor = a;
}

void Camera::moveX(int pixels)
{
    m_position -= Vector<int>(pixels, 0);
}

void Camera::moveY(int pixels)
{
    m_position -= Vector<int>(0, pixels);
}

Vector<int> Camera::getPosition() const
{
    return Vector<int>(x(), y());
}

int Camera::x() const
{
    // Feste X-Position: Kamera scrollt nur vertikal, nicht horizontal
    // Verwende die initiale X-Position aus m_position
    return m_position.x();
}

int Camera::y() const
{
    if(m_actor)
    {
        return m_actor->y();
    }
    else
    {
        return m_position.y();
    }
}

Camera::~Camera()
{
    // Nothing to do yet
}

} /* namespace jumper */
