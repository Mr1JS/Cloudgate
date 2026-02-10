/*
 *  MainWindow.cpp *
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */
#include "game/include/MainWindow.hpp"
#include "game/include/Level.hpp"

#include <SDL_image.h>
#include <iostream>
#include <cstring>
#include "game/include/MovingRenderable.hpp"
#include "game/include/Camera.hpp"

#define CAM_SPEED 6

namespace jumper
{

    MainWindow::MainWindow(std::string title, std::string level, int w, int h)
    {
        /// Init width and height
        m_width = w;
        m_height = h;

        /// Set pointer to NULL
        m_renderer = 0;

        /// Initialize SDL stuff
        initSDL();

        // Init level *after* renderer was built.
        m_level = new Level(this, level);
    }

    int MainWindow::w()
    {
        return m_width;
    }

    int MainWindow::h()
    {
        return m_height;
    }

    MainWindow::~MainWindow()
    {
        // Delete all resources
        delete m_level;

        // Quit SDL
        quitSDL();
    }

    void MainWindow::run()
    {
        bool quit = false;
        SDL_Event e;
        const Uint8 *currentKeyStates;

        // Start main loop and event handling
        while (!quit && m_renderer && m_level)
        {
            // Process events, detect quit signal for window closing
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }
            }

            // Clear screen and render level
            SDL_RenderClear(m_renderer);

            // Get key states and update level
            currentKeyStates = SDL_GetKeyboardState(NULL);
            m_level->render();
            m_level->update(currentKeyStates);

            // Update screen
            SDL_RenderPresent(m_renderer);
        }
    }

    void MainWindow::update(const Uint8 *keystates)
    {
        if (m_level && m_renderer)
        {
            m_level->update(keystates);
        }
    }

    void MainWindow::render()
    {
        if (m_level && m_renderer)
        {
            SDL_RenderClear(m_renderer);
            m_level->render();
            // In embedded/offscreen mode GameView reads back pixels directly
            // from the current render target. Presenting here would invalidate
            // the backbuffer before SDL_RenderReadPixels().
        }
    }

    Level *MainWindow::level()
    {
        return m_level;
    }

    SDL_Renderer *MainWindow::renderer()
    {
        return m_renderer;
    }

    void MainWindow::initSDL()
    {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            std::cout << "SDL could not initialize: " << SDL_GetError() << std::endl;
            return;
        }
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

        // Generate SDL main window (hidden for embedding in Qt)
        m_window = SDL_CreateWindow(
            "Jumper Main Window",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            m_width,
            m_height,
            SDL_WINDOW_HIDDEN);

        if (m_window == NULL)
        {
            std::cout << "SDL window could not be generated: " << SDL_GetError() << std::endl;
        }
        else
        {
            const char* videoDriver = SDL_GetCurrentVideoDriver();
            bool preferSoftwareRenderer = false;

            // Hidden offscreen windows + readback can be unreliable with some
            // Wayland/compositor setups. Prefer software there.
            if (videoDriver && std::strcmp(videoDriver, "wayland") == 0)
            {
                preferSoftwareRenderer = true;
            }

            // Manual override for debugging:
            // JUMPER_SDL_SOFTWARE=1 -> force software
            // JUMPER_SDL_SOFTWARE=0 -> allow accelerated first
            const char* forceSoftware = SDL_getenv("JUMPER_SDL_SOFTWARE");
            if (forceSoftware)
            {
                if (std::strcmp(forceSoftware, "1") == 0)
                {
                    preferSoftwareRenderer = true;
                }
                else if (std::strcmp(forceSoftware, "0") == 0)
                {
                    preferSoftwareRenderer = false;
                }
            }

            if (!preferSoftwareRenderer)
            {
                m_renderer = SDL_CreateRenderer(
                    m_window,
                    -1,
                    SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
                );
            }

            if (m_renderer == NULL)
            {
                if (!preferSoftwareRenderer)
                {
                    std::cout << "Hardware renderer not available, using software renderer: "
                              << SDL_GetError() << std::endl;
                }
                m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
            }

            if (m_renderer == NULL)
            {
                std::cout << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
            }
            else
            {
                std::cout << "SDL video driver: "
                          << (videoDriver ? videoDriver : "unknown")
                          << ", prefer software renderer: "
                          << (preferSoftwareRenderer ? "yes" : "no")
                          << std::endl;

                SDL_RendererInfo info;
                if (SDL_GetRendererInfo(m_renderer, &info) == 0)
                {
                    std::cout << "\n=== SDL Renderer Info ===\n";
                    std::cout << "Renderer name: " << (info.name ? info.name : "unknown") << "\n";
                    std::cout << "Flags: " << info.flags << "\n";
                    std::cout << "Max texture size: " << info.max_texture_width
                              << " x " << info.max_texture_height << "\n";

                    std::cout << "Supported texture formats (" << info.num_texture_formats << "):\n";
                    for (Uint32 i = 0; i < info.num_texture_formats; ++i)
                    {
                        std::cout << "  - "
                                  << SDL_GetPixelFormatName(info.texture_formats[i])
                                  << " (" << info.texture_formats[i] << ")\n";
                    }
                    std::cout << "=========================\n\n";
                }
                else
                {
                    std::cout << "SDL_GetRendererInfo failed: " << SDL_GetError() << std::endl;
                }

                // Set background color for renderer
                SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
            }
        }

        // Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
            std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        }
    }

    void MainWindow::quitSDL()
    {
        // Destroy window and renderer

        if (m_renderer)
        {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = 0;
        }

        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = 0;
        }

        // Quit SDL and SDL_Image
        IMG_Quit();
        SDL_Quit();
    }

} /* namespace jumper */
