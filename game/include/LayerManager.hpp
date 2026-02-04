/*
 *  LayerManager.cpp
 *  Created on: Jan 07, 2018
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2018 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef LAYERMANAGER_HPP_
#define LAYERMANAGER_HPP_

#include "Renderable.hpp"
#include "Camera.hpp"

#include <set>

namespace jumper
{

/**
 * @brief   Manages the rendering of renderables on different
 *          depth layers. The layer on which a renderable is
 *          rendered is specified via a layer id. The lowest
 *          layer has ID 0.
 *
 *          This class takes ownership of the provided pointers!
 *          Especially, all added renderables are destroyed
 *          when the LayerManager is deleted.
 */
class LayerManager
{
public:

    /// Typdef alias for layer tuple, i.e., renderable
    /// and used layer
    typedef std::pair <SDLRenderable*, int> Layer;

    /// Compare function for Layer tuples
    class compareLayer
    {
    public:
        bool operator() (const Layer& l1, const Layer& l2) const
        {
            return l1.second < l2.second;
        }
    };

    /// Creates a layer manager. Uses the given Camera
    /// pointer to render the elements with the correct
    /// offset
    LayerManager(Camera* cam);

    /// Adds a renderable to the given layer
    void addRenderable(SDLRenderable* r, int LayerID);

//    void removeRenderable(SDLRenderable* r, int LayerID);

    /// Renders all incrementally starting with layer 0
    void render();

    /// Destructor. Frees all managed renderables.
    virtual ~LayerManager();

private:

    /// Pointer to the virtual camera within the level
    Camera* m_camera;

    /// Container for the layers
    std::multiset<std::pair <SDLRenderable*, int>, compareLayer> m_renderables;

};

} /* namespace jumper */

#endif /* LAYERMANAGER_HPP_ */
