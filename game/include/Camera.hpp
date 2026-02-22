/*
 *  Camera.hpp
 *
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "MovingRenderable.hpp"
#include "Vector.hpp"

namespace jumper
{

/**
 * @brief A camera object to scroll within a level
 */
class Camera
{
public:

    /**
     * Constructs a camera with the given pixel offsets
     */
    Camera(int x = 0, int y = 0, int w = 0, int h = 0);

    /***
     * Moves the camera by the given amount of pixels in x-direction. Positive
     * values indicate movement to the right, negative values
     * move the camera to the left
     */
    void moveX(int pixels);

    /***
     * Moves the camera by the given amount of pixels in y-direction. Positive
     * values indicate downward movement, negative value indicate upward movement
     */
    void moveY(int pixels);

    /// set camera y-position
    void setY(int pixels);

    Vector<int> getPosition() const;

    /// Returns the current x-position
    int x() const;

    /// Returns the current y-position
    int y() const;

    /// Returns the camera width
    int width() const;

    /// Returns the camera height
    int height() const;

    /// Destructor
    virtual ~Camera();

    /// Sets an actor to follow
    void setFocus(MovingRenderable* actor);

    /// Updates the camera (for automatic scrolling)
    void update(double dt);

    /// set scroll speed for camera
    void setScrollSpeed(float scrollSpeed);

private:

    /// Position of the camera
    Vector<int>         m_position;

    /// Height of the associated window
    int                 m_width;

    /// Width of the associated window
    int                 m_height;

    /// An actor to follow
    MovingRenderable*   m_actor;
    
    /// Accumulator for smooth scrolling (stores fractional pixels)
    double              m_scrollAccumulator;

    /// speed of Camera scrolling up
    float               m_scrollSpeed = 8;
};

} /* namespace jumper */

#endif /* CAMERA_HPP_ */
