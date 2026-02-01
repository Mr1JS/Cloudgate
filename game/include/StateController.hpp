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

#include <SDL.h>
#include <QElapsedTimer>

#define MAX_HEARTS 3

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

    void startGameTime();

    /// Updates game time
    void updateGameTime();

    /// Reset runtime back to 0
    void resetGameTime();

    void decrementHp(int number = 1);

    void resetHp();

    bool isPaused();

    /// Returns current player HP
    int getHp() const { return m_playerHp; }

    /// renders hearts and game time on screen
    void render();

    /// Destructor of class StateController
    virtual ~StateController();

private:
    void resetHeartPosition();

    /// Player hp in hearts
    int m_playerHp;

    /// Current time elapsed in level
    int m_runtime;

    QElapsedTimer* m_timer;

    /// Whether level has started and runtime should be logged or not
    bool m_isRunning;

    std::array<SDLRenderable*, MAX_HEARTS> m_hearts;

    int m_heartWidth;

    MainWindow* m_mainWindow;
};

} /* namespace jumper */

#endif /* SRC_STATE_CONTROLLER_HPP_ */
