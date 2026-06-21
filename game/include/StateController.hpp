/**
 * @file StateController.hpp
 * @brief Defines the StateController class for managing game states, UI overlays and scoring
 */

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
#include "NumberDigit.hpp"

#include <list>
#include <SDL.h>
#include <QElapsedTimer>

#define MAX_HEARTS 3
#define RUNTIME_DIGITS 6
#define COIN_DIGITS 3

namespace jumper
{

/**
 * @brief Handles different game states.
 */
class StateController
{
public:
    /// Create an instance of StateController
    StateController(MainWindow* mainWindow, Level* level, std::string& filename);

    /// Initialize display of player hp as hearts in the top right corner
    void initHeartDisplay(SDL_Texture* heartTexture, int texWidth, int texHeight, int layer);

    /// Initialize display of level time display in the top left corner
    void initNumberDigits(SDL_Texture* heartTexture, int numFrames, int frameWidth, int frameHeight, int layer);

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

    /// Increase hp by given number (max. MAX_HEARTS). E.g. for red_potion.
    void incrementHp(int number = 1);

    /// Reset hp to default value
    void resetHp();

    /// Returns true if game is paused
    bool isPaused();

    /// Returns current player HP
    int getHp() const;

    /// returns current runtime in ms
    unsigned int getRuntime() const;

    /// Increase coin count
    void addCoin(int coinCount = 1);

    /// Get current coin count
    int getCoins() const;

    /// Activate super potion (blue_potion): 10 seconds boost + invincibility
    void activateSuperPotion();

    /// Return whether super potion is active
    bool isSuperPotionActive() const;

    /// Activates green potion (green_potion): tiles can be destroyed with head for 5 seconds (Mario-style)
    void activateBreakTilesMode();

    /// Returns whether Break-Tiles-Mode is active
    bool isBreakTilesModeActive() const;

    /// Destructor of class StateController
    virtual ~StateController();

private:

    /// reset position of heart graphics
    void resetHeartPosition();

    // updates runtime visuals
    void updateRuntime(unsigned int runtime);

    /// Player hp in hearts
    int                                         m_playerHp;

    /// Current time elapsed in level, as will be displayed on screen
    unsigned int                                m_runtime;

    /// Last time stored in m_timer (will be reset after a pause)
    unsigned int                                m_lastTimer;

    /// QElapsedTimer is used to measure time elapsed since game start
    QElapsedTimer*                              m_timer;

    /// Whether level has started and runtime should be logged or not
    bool                                        m_isRunning;

    /// heart sprites
    std::array<SDLRenderable*, MAX_HEARTS>      m_hearts;

    /// width of a heart sprite - used in calculating position of the heart sprites
    int                                         m_heartWidth;

    /// height of a heart sprite - used in calculating position of the coin sprites
    int                                         m_heartHeight;

    /// sprites of the individual timer digits
    std::array<NumberDigit*, RUNTIME_DIGITS>    m_runtimeDigits;

    /// reference to MainWindow class
    MainWindow*                                 m_mainWindow;

    /// reference to Level class
    Level*                                      m_level;
    
    /// sprites of the coin counter digits
    std::array<NumberDigit*, COIN_DIGITS>       m_coinTextures;

    /// Coins collected in level
    int                                         m_coins;

    /// time until end of super potion (SDL_GetTicks), 0 = not active
    unsigned int                                m_superPotionUntilTicks;

    /// time until end of Break-Tiles mode (green_potion), 0 = not active
    unsigned int                                m_breakTilesUntilTicks;
};

} /* namespace jumper */

#endif /* SRC_STATE_CONTROLLER_HPP_ */
