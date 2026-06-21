/*
 *  SDLRenderable.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

/**
 * @file SDLRenderable.hpp
 * @brief Defines the SDLRenderable base class for objects rendered using SDL
 */

#ifndef SDLRENDERABLE_HPP
#define SDLRENDERABLE_HPP

#include "Renderable.hpp"
#include "Vector.hpp"

#include <SDL.h>

namespace jumper
{

// MainWindow forward declaration
class MainWindow;

/**
 * @brief Implementation of basic rendering functionality
 *        using SDL functionalities.
 */
class SDLRenderable : public Renderable
{
public:

    /**
     * @brief SDLRenderable renders a SDL-Texture in a
     *        MainWindow
     *
     * @param mw            MainWindow for rendering
     * @param texture       Texture to render
     */
    SDLRenderable(MainWindow* mw = 0, SDL_Texture* texture = 0);

    /// Render method
    virtual void render() override;

    /// Width
    virtual int w() const override;

    /// Height
    virtual int h() const override;

    /// X position in screen coordinates
    virtual int x() const override;

    /// Y position in screen coordintes
    virtual int y() const override;

    /// Returns the current position
    Vector<int> position() const override;

    /// Sets the position of the renderable in world coordinates
    void setPosition(const Vector<int>& v) override;

    /// Renders the tile array according to the given offset
    void setOffset(const Vector<int>& offset);

    /// Scales the renderable to fit the window while maintaining aspect ratio
    void scaleToWindow();

    /// Destructor
    virtual ~SDLRenderable() {}

protected:

    /// Computes the upper left screen coordinates taking
    /// camera offset and window dimensions into accout.
    Vector<int> computeTargetPosition() const;

    /// Checks whether rendering is possible
    inline bool readyToRender()
    {
        return m_mainWindow && m_texture;
    }

    /// Computes the source rect from the internal texture
    void computeSourceRect();

    /// MainWindow pointer
    MainWindow*         m_mainWindow;

    /// Position in screen coordinates
    Vector<int>         m_position;

    /// Renderer
    SDL_Renderer*		m_renderer;

    /// A texture object
    SDL_Texture*		m_texture;

    /// Source rect in the texture
    SDL_Rect 			m_sourceRect;

    /// Position on the target renderer
    SDL_Rect            m_targetRect;

    /// Rendering offset of the tile set. Used when
    /// moving the camera
    Vector<int>      	m_offset;

    /// Rendering offset to accout for window dimensions.
    /// Used to keep camera viewport centered.
    Vector<int>			m_windowOffset;

};

} // namespace jumper

#endif // SDLRENDERABLE_HPP
