/*
 *  StaticRenderable.hpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef STATICRENDERABLE_HPP
#define STATICRENDERABLE_HPP

#include "SDLRenderable.hpp"

namespace jumper {

/**
 * @brief   The StaticRenderable class is an interface for all static
 *          SDL objects that can be rendered.
 */
class StaticRenderable: public SDLRenderable
{
public:

    /// Creates a static renderable with the given texture for
    /// the given MainWindow
    StaticRenderable(MainWindow* mw, SDL_Texture* texture = 0) : SDLRenderable(mw, texture) {}

    /// Destructor
    virtual ~StaticRenderable() {}
};

} // namespace jumper;

#endif // STATICRENDERABLE_HPP
