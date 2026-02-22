/**
 * @file AnimatedRenderable.hpp
 * @brief Defines the AnimatedRenderable class for objects with sprite-based animations
 */

/*
 *  AnimatedRenderable.cpp
 *
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */


#ifndef SRC_ANIMATEDRENDERABLE_HPP_
#define SRC_ANIMATEDRENDERABLE_HPP_

#include <SDL.h>

#include <string>
#include <iostream>
#include "MovingRenderable.hpp"

namespace jumper
{

class MainWindow;

/**
 * @brief Base class of renderables with several animation frames
 */
class AnimatedRenderable: public MovingRenderable
{
public:

    /***
     * Constructs an animated renderable from the given \ref filename
     * for the internal \ref renderer
     * @param renderer		A pointer to a SDL renderer struct
     * @param filename		A filename with animation definitions
     */
    AnimatedRenderable(MainWindow* mainWindow, std::string filename);

    /***
     * Constructs an animated renderable from given renderer, texture
     * and frame information.
     * @param renderer		A pointer to a SDL renderer struct
     * @param filename		A filename with animation definitions
     */
    AnimatedRenderable(MainWindow* mainWindow, SDL_Texture* texture, int frameWidth, int frameHeight, int numFrames);


    /**
     * Destructor.
     */
    virtual ~AnimatedRenderable();

    /**
     * Renders the current frame
     */
    virtual void render() override;

    /**
     * Moves the source rect to the next frame
     */
    virtual void nextFrame();

    /**
     * Sets frames per second for animation
     */
    void setFPS(int frames);

protected:

    ///	Number of frames in the animation
    int				m_numFrames;

    /// Current frame number
    int				m_currentFrame;

    /// The width of a frame
    int 			m_frameWidth;

    /// The height of a frame
    int				m_frameHeight;

    /// Ticks count when the last frame was rendered
    Uint32			m_lastRenderTicks;

    /// Timeout between frames
    Uint32			m_frameTimeout;

};

} /* namespace jumper */

#endif /* SRC_ANIMATEDRENDERABLE_HPP_ */
