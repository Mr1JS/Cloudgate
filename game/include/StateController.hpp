/*
 *  StateController.hpp
 *
 *  Created on: Jan 21, 2026
 *      Author: Merlin Aubel
 */
#ifndef SRC_STATE_CONTROLLER_HPP_
#define SRC_STATE_CONTROLLER_HPP_

#include "MainWindow.hpp"
#include "SDLRenderable.hpp"
#include "TimerDigit.hpp"

#include <SDL.h>
#include <QElapsedTimer>

#define MAX_HEARTS 3
#define RUNTIME_DIGITS 6

namespace jumper
{

/**
 * @brief Handles different game states.
 */
class StateController
{
public:
    /// Create an instance of StateController
    StateController(MainWindow* mainWindow, std::string& filename);

    /// Initialize display of player hp as hearts in the top right corner
    std::array<SDLRenderable*, MAX_HEARTS> initHeartDisplay(SDL_Texture* heartTexture, int texWidth, int layer);

    /// Initialize display of level time display in the top left corner
    std::array<TimerDigit*, RUNTIME_DIGITS> initTimerDigits(SDL_Texture* heartTexture, int numFrames, int frameWidth, int frameHeight, int layer);

    /// Start or resume game
    void startGame();

    /// Updates game time, including display
    void updateGameTime();

    /// Reset runtime back to 0
    void resetGame();

    /// Stop and pause game
    void stop();

    /// Returns current hp count
    int getHp();

    /// Decrement hp by optionally given number
    /// @param number (optional) amount of hp to decrease (default: 1)
    void decrementHp(int number = 1);

    /// Reset hp to default value
    void resetHp();

    /// Returns true if game is paused
    bool isPaused();

    /// Returns current player HP
    int getHp() const;

    unsigned int getRuntime() const;

    /// Destructor of class StateController
    virtual ~StateController();

private:
    void resetHeartPosition();

    // updates runtime visuals
    void updateRuntime(unsigned int runtime);

    /// Player hp in hearts
    int m_playerHp;

    /// Current time elapsed in level, as will be displayed on screen
    unsigned int m_runtime;

    /// Last time stored in m_timer (will be reset after a pause)
    unsigned int m_lastTimer;

    /// QElapsedTimer is used to measure time elapsed since game start
    QElapsedTimer* m_timer;

    /// Whether level has started and runtime should be logged or not
    bool m_isRunning;

    /// heart sprites
    std::array<SDLRenderable*, MAX_HEARTS> m_hearts;

    /// width of a heart sprite - used in calculating position of the sprites
    int m_heartWidth;

    /// sprites of the individual timer digits
    std::array<TimerDigit*, RUNTIME_DIGITS> m_runtimeDigits;

    /// reference to MainWindow class
    MainWindow* m_mainWindow;
};

} /* namespace jumper */

#endif /* SRC_STATE_CONTROLLER_HPP_ */
