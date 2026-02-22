/**
 * @file Actor.hpp
 * @brief Defines the Actor class for movable game characters that interact and collide with other objects
 */

/*
 *  Actor.hpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef SRC_ACTOR_HPP_
#define SRC_ACTOR_HPP_

#include "AnimatedRenderable.hpp"
#include "ActorForces.hpp"

#include <string>
#include <thread>
#include <chrono>

namespace jumper
{

class MainWindow;

/**
 * @brief 	A class to represent an object, that moves in a level
 * 			and interacts (collides) with other objects
 */
class Actor : public AnimatedRenderable
{
public:

    /// Creates an actor for the given renderer from the given filename
    Actor(MainWindow* mainWindow, std::string filename);

    /// Creates an actor from a texture and animation information
    Actor(MainWindow* renderer, SDL_Texture* texture, int frameWidth, int frameHeight, int numFrames);

    /// Destructor
    virtual ~Actor();

    /// Handles collision detection and response with another actor
    virtual void getCollision(Actor& other);

    /// Renders the actor sprite to screen
    virtual void render() override;

    /// Sets the physical properties (forces, gravity, etc.) for this actor
    void setPhysics(ActorForces p);

    /**
     * Returns the player's physical properties
     */
    ActorForces &forces();

    /**
     * True, if the player touches ground
     */
    bool onGround() const;

    /**
     * Sets the on ground flag to the given status
     */
    void setOnGround(bool m_onGround);

    /**
     * Sets the jump state
     */
    void setJumping(bool jump);

    /**
     * Returns the jump state
     */
    bool jumping();

    /**
     * Returns the pixel position at which jumping was started
     */
    int jumpStart();

    /// Indicates that the actor wants to start to jump
    void setWantsToJump(bool j);

    /// Returns weather the actor wants to start to jump
    bool wantsToJump();

    /// Set Blink-Status: blink = Invincibility (red), superPotion = blue, breakTilesMode = green
    void setBlinking(bool blink, bool superPotion = false, bool breakTilesMode = false);

protected:

    /// Returns the time elapsed since the last call of this function
    float getElapsedTime();

    /// True if the player touches ground
bool     			    m_onGround;

    /// True if the player is jumping
    bool     			m_jumping;

    /// Indicates if the actors wants to start jumping
    bool				m_wantsToJump;

    /// Pixel position where jumping has started
    int     			m_jumpStart;

    /// The physical properties of the player
    ActorForces         m_forces;

    /// Blink-Status (for Invincibility)
    bool                m_blinking;

    /// Super-Potion active (blinking blue)
    bool                m_superPotionActive;
    
    /// Green-Potion / Break-Tiles active (blinking green)
    bool                m_breakTilesModeActive;
    
    /// Salto-Rotation in Grad (nur bei Super-Trank in der Luft)
    double              m_saltoRotation;
    
    /// last time a salto was performed in ticks
    unsigned int        m_lastSaltoTicks;

};

} /* namespace jumper */

#endif /* SRC_ACTOR_HPP_ */
