/*
 *  LayerManager.hpp
 *  Created on: Jan 07, 2018
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2018 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "include/LayerManager.hpp"

#include <iostream>
using std::cout;
using std::endl;

namespace jumper
{

LayerManager::LayerManager(Camera* cam)
{
    m_camera = cam;
}

void LayerManager::addRenderable(SDLRenderable *r, int LayerID)
{
    m_renderables.insert(Layer(r,LayerID));
}


void LayerManager::render()
{
    for(auto it: m_renderables)
    {
         it.first->setOffset(m_camera->getPosition());
         it.first->render();
    }
}

LayerManager::~LayerManager()
{
    for(auto it: m_renderables)
    {
        delete (it.first);
    }
}

} /* namespace jumper */
