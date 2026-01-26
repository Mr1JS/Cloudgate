/*
 *  AnimatedRenderable.hpp
 *
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "game/include/AnimatedRenderable.hpp"
#include "game/include/MainWindow.hpp"
#include "game/include/Util.hpp"

#include <fstream>
#include <iostream>

using std::cout;
using std::endl;

namespace jumper
{

AnimatedRenderable::AnimatedRenderable(MainWindow* mw, std::string filename)
    : MovingRenderable(mw), m_numFrames(0), m_currentFrame(0), m_frameWidth(0), m_frameHeight(0)
{
    std::ifstream ifs(filename.c_str());
    std::string textureFileName;
    if(ifs.good())
    {
        ifs >> textureFileName >> m_frameWidth >> m_frameHeight >> m_numFrames;

        std::string path = GetPathFromFileName(filename);

        m_texture = LoadTexture(m_mainWindow->renderer(), path + 	textureFileName);
    }
    else
    {
        std::cout << "Unable to open file " << filename << std::endl;
    }

    // Initialize source rect
    m_sourceRect.x = 0;
    m_sourceRect.y = 0;
    m_sourceRect.w = m_frameWidth;
    m_sourceRect.h = m_frameHeight;

    m_targetRect.w = m_frameWidth;
    m_targetRect.h = m_frameHeight;

}

AnimatedRenderable::AnimatedRenderable(MainWindow* mw, SDL_Texture* texture, int frameWidth, int frameHeight, int numFrames)
    : MovingRenderable(mw, texture),
      m_numFrames(numFrames),
      m_currentFrame(0),
      m_frameWidth(frameWidth),
      m_frameHeight(frameHeight),
      m_lastRenderTicks(0),
      m_frameTimeout(0)
{
    // Initialize source recnt
    m_sourceRect.x = 0;
    m_sourceRect.y = 0;
    m_sourceRect.w = m_frameWidth;
    m_sourceRect.h = m_frameHeight;

    m_targetRect.w = m_frameWidth;
    m_targetRect.h = m_frameHeight;
}

AnimatedRenderable::~AnimatedRenderable() {}


void AnimatedRenderable::nextFrame()
{
    Uint32 ticks = SDL_GetTicks();
    float time =  (ticks - m_lastRenderTicks);

    if(time > m_frameTimeout)
    {
        // Check and increase frame counter
        if(m_currentFrame + 1 < m_numFrames)
        {
            m_currentFrame++;
        }
        else
        {
            m_currentFrame = 0;
        }

        // Setup source rect
        m_sourceRect.x = m_currentFrame * m_frameWidth;

        // Save current tick count
        m_lastRenderTicks = ticks;
    }
}

void AnimatedRenderable::render()
{
    if(readyToRender())
    {
        nextFrame();
        MovingRenderable::render();
    }
}

void AnimatedRenderable::setFPS(int frames)
{
    m_frameTimeout = (Uint32)(1000.0 / frames);
}

} /* namespace jumper */


