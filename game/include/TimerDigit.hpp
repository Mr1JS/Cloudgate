/*
 *  TimerDigit.hpp
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

//class MainWindow;

/**
 * @brief 	A class to represent an object, that moves in a level
 * 			and interacts (collides) with other objects
 */
class TimerDigit : public AnimatedRenderable
{
public:

    /// Creates an digit for the given renderer from the given filename
    TimerDigit(MainWindow* mainWindow, std::string filename);

    /// Creates an digit from a texture and animation information
    TimerDigit(MainWindow* renderer, SDL_Texture* texture, int frameWidth, int frameHeight, int numFrames);

    /// Destructor
    virtual ~TimerDigit();

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
