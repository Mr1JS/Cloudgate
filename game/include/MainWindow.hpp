/*
 *  MainWindow.hpp *
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef SRC_MAINWINDOW_HPP_
#define SRC_MAINWINDOW_HPP_

#include <string>
#include <SDL.h>
#include "LayerManager.hpp"
#include "TileSet.hpp"

namespace jumper
{

// Forward declaration of Level class type
class Level;


/**
 * @brief The main window of the game
 */
class MainWindow
{
public:

    /***
     * Creates a main window with given \ref title, width \ref w and height \ref h
     *
     * @param title		Title of the window
     * @param level     XML definition of level attributes
     * @param w			Width
     * @param h			Height
     */
    MainWindow(std::string title, std::string level, int w, int h);

    /***
     * Destructor.
     */
    ~MainWindow();

    /***
     * Handles user events and renders the current level.
     */
    void run();

    /***
     * Updates the game logic (without rendering)
     */
    void update(const Uint8* keystates);

    /***
     * Renders the current level
     */
    void render();

    /***
     * Gets the current SDL renderer
     */
    SDL_Renderer* renderer();
    
    /***
     * Gets the level pointer
     */
    Level* level();

    /// Returns the current width of the window
    int w();

    /// Returns the current height of the window
    int h();

private:

    /// Initializes all needed SDL resources
    void initSDL();

    /// Quits SDL and frees all resources
    void quitSDL();

    /// SDL renderer struct
    SDL_Renderer* 		m_renderer;

    /// SDL main window struct
    SDL_Window*			m_window;

    /// Optional offscreen render target for deterministic readback
    SDL_Texture*        m_offscreenTarget;

    /// Window width
    int					m_width;

    /// Window height
    int					m_height;

    /// A pointer to a level object
    Level*				m_level;

};

} /* namespace jumper */

#endif /* SRC_MAINWINDOW_HPP_ */
