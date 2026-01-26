#include "game/include/StateController.hpp"

#include <fstream>
#include "game/include/SDLRenderable.hpp"
#include "game/include/Util.hpp"

namespace jumper
{

StateController::StateController(MainWindow* mainWindow, std::string& filename)
{
    m_mainWindow = mainWindow;
    m_isRunning = false;
    m_runtime = 0;
    m_playerHp = 3;
    m_timer = new QElapsedTimer();
    m_heartFileName = getHeartFileName(filename);
    m_heartTexture = LoadTexture(m_mainWindow->renderer(), m_heartFileName);
}

std::string StateController::getHeartFileName(const std::string& filename)
{
    // Read meta data from level file
    std::ifstream in(filename.c_str());
    std::string texFileName, actorFileName, heartFileName;

    // Get path from given .lvl file
    size_t position = filename.find_last_of("/\\");
    std::string path = filename.substr(0, position + 1);

    if(in.good())
    {
        in >> texFileName;
        in >> actorFileName;
        in >> heartFileName;
    }
    else
    {
        std::cout << "Unable to open file " << filename << std::endl;
    }

    in.close();

    return path + heartFileName;
}

bool StateController::isPaused() {
    return !m_isRunning;
}

void StateController::startGameTime()
{
    m_isRunning = true;
    m_timer->start();
}

void StateController::updateGameTime()
{
    if (m_isRunning) {
        m_runtime = m_timer->elapsed();
    }

    if (m_runtime % 100 == 1) {
        int min = m_runtime/1000/60;
        int sec = m_runtime/1000 - min*60;
        int ms = m_runtime - sec*1000;
        std::cout << "Current time: " << min << ":" << sec << ":" << ms << std::endl;
    }
}

void StateController::resetGameTime()
{
    m_runtime = 0;
    m_isRunning = false;
}

void StateController::decrementHp(int number)
{
    m_playerHp -= number;
}

void StateController::render()
{
    nextFrame();
    SDL_Rect target;
    SDL_Rect sourceRect;
    sourceRect.x = 0;
    sourceRect.y = 0;
    sourceRect.w = 15;
    sourceRect.h = 15

    target.x = 100;
    target.y =  20;
    target.w = 32;
    target.h = 32;
    SDL_RenderCopy( m_mainWindow->renderer(), m_heartTexture, &sourceRect, &target, 0, NULL);

    for (int i = 0; i < m_playerHp; i++)
    {
        SDLRenderable* renderedHeart;
        renderedHeart = new SDLRenderable(m_mainWindow, m_heartTexture);
        renderedHeart->setPosition(Pixel(m_mainWindow->w() - i * (10 + renderedHeart->w()), 10));
    }
}

StateController::~StateController()
{}

} /* namespace jumper */

