/*
 *  Renderable.hpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef RENDERALBE_H
#define RENDERALBE_H

#include "Vector.hpp"

namespace jumper
{

/**
 * @brief Interface definition for all renderable objects
 */
class Renderable
{
public:
    /// Render method
    virtual void render() = 0;

    /// Width
    virtual int w() const = 0;

    /// Height
    virtual int h() const = 0;

    /// X position in screen coordinates
    virtual int x() const = 0;

    /// Y position in screen coordintes
    virtual int y() const = 0;

    /// Sets the position of the renderable to \ref v (in screen coodinates)
    virtual void setPosition(const Vector<int>& v) = 0;

    /// Returns the current position in screen coordinates
    virtual Vector<int> position() const = 0;

    /// Destructor
    virtual ~Renderable() {}
};

} // namespace jumper

#endif // RENDERALBE_H
