/**
 * @file NumberDigit.cpp
 * @brief Implementation of the NumberDigit class for visual representation of single digits
 *        for score display, timers and other numeric UI elements in the game
 */

#include "game/include/NumberDigit.hpp"

namespace jumper
{

NumberDigit::NumberDigit(MainWindow* mainWindow, std::string filename)
    : AnimatedRenderable(mainWindow, filename)
{
    m_value = 0;
    setWorldPosition(Vector2f(100, 0));
}

NumberDigit::NumberDigit(MainWindow* mainWindow, SDL_Texture* texture, int frameWidth, int frameHeight, int numFrames)
    : AnimatedRenderable(mainWindow, texture, frameWidth, frameHeight, numFrames)
{
    m_value = 0;
    setWorldPosition(Vector2f(100, 0));
}

NumberDigit::~NumberDigit()
{
    // nothing to do yet
}

void NumberDigit::render()
{   
    while (m_value != m_currentFrame)
    {
        //std::cout << "Advancing from frame " << m_currentFrame << " to value " << m_value << std::endl;
        nextFrame();
    }
    SDL_Rect target;

    target.x = m_position.x();
    target.y = m_position.y();
    target.w = m_frameWidth;
    target.h = m_frameHeight;

    //std::cout << "Drawing frame " << m_currentFrame << " at coordinates " << target.x << "/" << target.y << std::endl;

    SDL_RenderCopy(m_mainWindow->renderer(), m_texture,
                    &m_sourceRect, &target);
}

void NumberDigit::nextFrame()
{
    // Check and increase frame counter
    if (m_currentFrame + 1 < m_numFrames)
    {
        m_currentFrame++;
    }
    else
    {
        m_currentFrame = 0;
    }

    // Setup source rect
    m_sourceRect.x = m_currentFrame * m_frameWidth;
}

void NumberDigit::setValue(unsigned int value)
{
    m_value = value % m_numFrames;
}

} // namespace jumper

