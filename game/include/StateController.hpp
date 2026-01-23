/*
 *  StateController.hpp
 *
 *  Created on: Jan 21, 2026
 *      Author: Merlin Aubel
 */
#ifndef SRC_STATE_CONTROLLER_HPP_
#define SRC_STATE_CONTROLLER_HPP_

#include "MainWindow.hpp"

#include <SDL.h>
#include <QElapsedTimer>

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

    void startGameTime();

    /// Updates game time
    void updateGameTime();

    /// Reset runtime back to 0
    void resetGameTime();

    void decrementHp(int number = 1);

    void resetHp();

    bool isPaused();

    /// renders hearts and game time on screen
    void render();

    /// Destructor of class StateController
    virtual ~StateController();

private:
    /// Get string of filename for heart texture
    std::string getHeartFileName(const std::string& filename);
    
    /// Player hp in hearts
    int m_playerHp;

    /// Current time elapsed in level
    int m_runtime;
    QElapsedTimer* m_timer;

    /// Whether level has started and runtime should be logged or not
    bool m_isRunning;

    std::string m_heartFileName;

    SDL_Texture* m_heartTexture;

    MainWindow* m_mainWindow;
};

} /* namespace jumper */

#endif /* SRC_STATE_CONTROLLER_HPP_ */
