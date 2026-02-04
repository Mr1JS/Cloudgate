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

    std::array<SDLRenderable*, MAX_HEARTS> addHeartTexture(SDL_Texture* heartTexture, int texWidth, int layer);

    std::array<TimerDigit*, RUNTIME_DIGITS> addDigits(SDL_Texture* heartTexture, int numFrames, int frameWidth, int frameHeight, int layer);

    void startGame();

    /// Updates game time
    void updateGameTime();

    /// Reset runtime back to 0
    void resetGame();

    void stop();

    int getHp();

    void decrementHp(int number = 1);

    void resetHp();

    bool isPaused();

    /// Returns current player HP
    int getHp() const;

    unsigned int getRuntime() const;

    /// renders hearts and game time on screen
    void render();

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

    std::array<SDLRenderable*, MAX_HEARTS> m_hearts;

    int m_heartWidth;

    std::array<TimerDigit*, RUNTIME_DIGITS> m_runtimeDigits;

    MainWindow* m_mainWindow;
};

} /* namespace jumper */

#endif /* SRC_STATE_CONTROLLER_HPP_ */
