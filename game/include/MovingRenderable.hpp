/*
 *  MovingRenderable.hpp *
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef ACTOR_H
#define ACTOR_H

#include "SDLRenderable.hpp"
#include "Vector.hpp"

#include <SDL.h>

namespace jumper
{

// Forward declaration of MainWindow class type
class MainWindow;

/**
 * @brief Represents an renderable that can be  in the game.
 */
class MovingRenderable: public SDLRenderable
{
public:


    MovingRenderable(MainWindow* mw = 0, SDL_Texture* t = 0) : SDLRenderable(mw, t) {}

    /**
     * @brief	Creates an actor from the given texture
     *
     * @param	mainWindow	A pointer to a MainWindow instance
     * @param	texture		A pointer to a SDL texture for the actor
     */
    MovingRenderable(MainWindow* mainWindow, std::string filename);

    /**
     * @brief	Creates an actor from the given texture
     *
     * @param	mainWindow	A pointer to a MainWindow instance
     * @param	texture		A pointer to a SDL texture for the actor
     * @param	position	Initial position of the actor (in world coordinates)
     * @param	angle		Initial rotation angle of the actor
     */
    MovingRenderable(MainWindow* mainWindow, SDL_Texture* texture,
                     const Vector<double>& position, const double angle = 0.0);

    /// Rotates the actor by \ref angle degrees
    void operator*=(double angle);

    /// Adds \ref vec to the current position of the actor
    void operator+=(const Vector<double>& vec);

    /// Subtracts \ref vec from the current position of the actor
    void operator-=(const Vector<double>& vec);

    /// Sets the current rotation to \ref angle
    void setAngle(const double& angle);

    /// Returns the current rotation angle
    double angle() const;

    /// Sets the current Position in world coordinates
    void setWorldPosition(const Vector<double>& position);

    /// Returns the current world position
    Vector<double> worldPosition() const;

    /// Sets the current velocity
    void setVelocity(const Vector<double>& velocity);

    /// Returns the current velocity
    Vector<double>& velocity();

    /// Updates the world position according to current velocity
    virtual void move();

    /// Rendering
    virtual void render() override;

    virtual ~MovingRenderable() {};

protected:

    /// Position of the renderable in world coordinates
    Vector<double>  m_worldPosition;

    /// Velocity
    Vector<double>  m_velocity;

    /// Rotation angle in degrees
    double          m_rotation;

    /// Compute screen coordinates from world coordinates
    void computeScreenCoordinates();

};


}

#endif // ACTOR_H
