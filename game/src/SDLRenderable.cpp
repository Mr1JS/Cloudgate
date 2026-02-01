/*
 *  SDLRenderable.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "include/GameController.hpp"
#include "game/include/MainWindow.hpp"

#include <iostream>

namespace jumper {

SDLRenderable::SDLRenderable(MainWindow *mw, SDL_Texture *texture)
    : m_mainWindow(mw), m_texture(texture), m_windowOffset(mw->w() / 2, mw->h() / 2)
{
    computeSourceRect();

    m_targetRect.x = 0;
    m_targetRect.y = 0;
}

void SDLRenderable::setOffset(const Vector<int>& offset)
{
    m_offset = offset;
}

void SDLRenderable::computeSourceRect()
{
    m_sourceRect = { 0, 0, 0, 0 };
    if(m_texture)
    {
        Uint32 format;
        int access, w, h;
        SDL_QueryTexture(m_texture, &format, &access, &w, &h);

        m_sourceRect.w = w;
        m_sourceRect.h = h;

        // Set target to same dimensions as default initialization
        m_targetRect.w = w;
        m_targetRect.h = h;
    }


}

void SDLRenderable::render()
{
    if(readyToRender())
    {
        SDL_Rect dst = m_targetRect;
        SDL_RenderCopyEx(
                    m_mainWindow->renderer(),
                    m_texture,
                    &m_sourceRect, &dst,
                    0,
                    NULL, SDL_FLIP_NONE);
    }

}

void SDLRenderable::setPosition(const Vector<int>& v)
{
    m_position = v;
    m_targetRect.x = m_position.x();
    m_targetRect.y = m_position.y();
}

int SDLRenderable::w() const
{
    return m_targetRect.w;
}

int SDLRenderable::h() const
{
    return m_targetRect.h;
}

int SDLRenderable::x() const
{
    //std::cout << m_position << std::endl;
    return m_position.x();
}

int SDLRenderable::y() const
{
    return m_position.y();
}

Vector<int> SDLRenderable::computeTargetPosition() const
{
	return Vector<int>(m_position - m_offset + m_windowOffset);
}


Vector<int> SDLRenderable::position() const
{
    return m_position;
}

void SDLRenderable::scaleToWindow()
{
    if (!m_mainWindow || m_sourceRect.w == 0 || m_sourceRect.h == 0)
    {
        return;
    }

    int windowW = m_mainWindow->w();
    int windowH = m_mainWindow->h();

    // calculate scaling, keep aspect ratio
    float scaleW = static_cast<float>(windowW) / m_sourceRect.w;
    float scaleH = static_cast<float>(windowH) / m_sourceRect.h;
    float scale = std::min(scaleW, scaleH);

    m_targetRect.w = static_cast<int>(m_sourceRect.w * scale);
    m_targetRect.h = static_cast<int>(m_sourceRect.h * scale);

    // center in window
    m_targetRect.x = (windowW - m_targetRect.w) / 2;
    m_targetRect.y = (windowH - m_targetRect.h) / 2;
}

} // namespace jumper
