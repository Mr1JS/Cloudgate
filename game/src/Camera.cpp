/**
 * @file Camera.cpp
 * @brief Implementation of the Camera class for dynamic viewport management, scrolling,
 *        player tracking and screen boundaries within the level
 */

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

#include "game/include/Camera.hpp"

#include <iostream>
using namespace std;

namespace jumper
{

Camera::Camera(int x, int y, int w, int h)
    : m_position(x,y), m_scrollAccumulator(0.0)
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

void Camera::setY(int pixels)
{
    m_position = Vector<int>(x(), pixels);
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
    // Kamera scrollt automatisch nach oben, folgt nicht mehr dem Actor
    return m_position.y();
}

void Camera::update(double dt)
{
    // Kamera scrollt langsam nach oben (Y-Position verringern)
    
    // Akkumuliere die Bewegung (für smooth scrolling bei kleinen Werten)
    m_scrollAccumulator += m_scrollSpeed * dt;
    
    // Wenn wir mindestens 1 Pixel bewegt haben, wende die Bewegung an
    if (m_scrollAccumulator >= 1.0)
    {
        int pixelsToMove = static_cast<int>(m_scrollAccumulator);
        m_position.setY(m_position.y() - pixelsToMove);
        m_scrollAccumulator -= pixelsToMove;  // Behalte den Rest für den nächsten Frame
    }
}

void Camera::setScrollSpeed(float scrollSpeed)
{
    m_scrollSpeed = scrollSpeed;
}

int Camera::width() const
{
    return m_width;
}

int Camera::height() const
{
    return m_height;
}

Camera::~Camera()
{
    // Nothing to do yet
}

} /* namespace jumper */
