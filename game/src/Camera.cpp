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
    // Camera scrolls slowly upward (decrease Y-position)
    
    float currentSpeed = m_scrollSpeed;
    
    // Check if we are following an actor
    if(m_actor != nullptr)
    {
        // Calculate the relative Y-position of the actor to the camera
        // (in screen coordinates)
        int actorWorldY = static_cast<int>(m_actor->worldPosition().y());
        int actorScreenY = actorWorldY - m_position.y();
        
        // Calculate threshold at 50% of screen height
        float thresholdHeight = m_height * 0.5f;
        bool isAboveThreshold = actorScreenY < thresholdHeight;
        
        // Debug output every 60 frames
        static int debugCounter = 0;
        if(debugCounter++ % 60 == 0)
        {
            cout << "Camera: ActorScreenY=" << actorScreenY 
                 << ", Threshold(50%)=" << thresholdHeight
                 << ", BoostTime=" << m_boostTimeRemaining << "s" 
                 << ", IsAbove=" << (isAboveThreshold ? "YES" : "NO") << endl;
        }
        
        // Trigger boost whenever actor is in upper 50% and no boost is currently active
        if(isAboveThreshold && m_boostTimeRemaining <= 0.0f)
        {
            m_boostTimeRemaining = m_boostDuration;
            cout << ">>> BOOST TRIGGERED! Actor at " << actorScreenY << " pixels (threshold: " << thresholdHeight << ")" << endl;
        }
        
        // Apply boost if active
        if(m_boostTimeRemaining > 0.0f)
        {
            currentSpeed *= (1.0f + m_speedBoostFactor);
            m_boostTimeRemaining -= dt;
            
            if(m_boostTimeRemaining <= 0.0f)
            {
                cout << ">>> BOOST ENDED, back to normal speed" << endl;
            }
        }
    }
    else
    {
        static bool warned = false;
        if(!warned)
        {
            cout << "WARNING: No actor to follow!" << endl;
            warned = true;
        }
    }
    
    // Accumulate the movement (for smooth scrolling with small values)
    m_scrollAccumulator += currentSpeed * dt;
    
    // If we have moved at least 1 pixel, apply the movement
    if(m_scrollAccumulator >= 1.0)
    {
        int pixelsToMove = static_cast<int>(m_scrollAccumulator);
        m_position.setY(m_position.y() - pixelsToMove);
        m_scrollAccumulator -= pixelsToMove;  // Keep the remainder for the next frame
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
