/*
 *  NumberDigit.hpp
 *  Created on: Feb 01, 2026
 *      Author: Merlin Aubel
 */

#ifndef SRC_DIGIT_HPP_
#define SRC_DIGIT_HPP_

#include "AnimatedRenderable.hpp"
#include "MainWindow.hpp"

#include <string>
#include <thread>
#include <chrono>

namespace jumper
{

/**
 * @brief A class to represent a number digit to show on screen (e.g. timer, coin count)
 */
class NumberDigit : public AnimatedRenderable
{
public:

    /// Creates an digit for the given renderer from the given filename
    NumberDigit(MainWindow* mainWindow, std::string filename);

    /// Creates an digit from a texture and animation information
    NumberDigit(MainWindow* renderer, SDL_Texture* texture, int frameWidth, int frameHeight, int numFrames);

    /// Destructor
    virtual ~NumberDigit();

    /// Renders the digits
    virtual void render() override;

    virtual void nextFrame() override;

    void setValue(unsigned int value);

protected:

    // value of the digit
    unsigned int m_value;
};

} /* namespace jumper */

#endif /* SRC_DIGIT_HPP_ */
