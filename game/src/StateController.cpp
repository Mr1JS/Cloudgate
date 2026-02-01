#include "game/include/StateController.hpp"

#include <fstream>
#include "game/include/Util.hpp"

namespace jumper
{

StateController::StateController(MainWindow* mainWindow, std::string& filename)
: m_hearts{}
{
    m_mainWindow = mainWindow;
    m_isRunning = false;
    m_runtime = 0;
    m_playerHp = MAX_HEARTS;
    m_timer = new QElapsedTimer();
    m_heartWidth = -1;
}


std::array<SDLRenderable*, MAX_HEARTS> StateController::addHeartTexture(SDL_Texture* heartTexture, int texWidth, int layer)
{
//    m_hearts = new SDLRenderable[MAX_HEARTS];
    std::cout << "Loading heart textures" << std::endl;
    m_heartWidth = texWidth;
    for (int i = 0; i < MAX_HEARTS; i++)
    {
        if (m_hearts[i])
        {
            std::cout << "Heart " << i << " already instanciated! Skipping." << std::endl;
            continue;
        }
        m_hearts[i] = new SDLRenderable(m_mainWindow, heartTexture);
    }

    resetHeartPosition();

    std::cout << "Loading hearts done" << std::endl;

    return m_hearts;
}

void StateController::resetHeartPosition()
{
    for (int i = 0; i < MAX_HEARTS; i++)
    {
        if (m_hearts[i])
        {
            Vector v = Vector(m_mainWindow->w() - m_heartWidth*(i+1), 10);
            m_hearts[i]->setPosition(v);
        }
    }
}

bool StateController::isPaused()
{
    return !m_isRunning;
}

void StateController::startGameTime()
{
    m_isRunning = true;
    m_timer->start();
}

void StateController::updateGameTime()
{
    if (!m_isRunning)
    {
        return;
    }

    m_runtime = m_timer->elapsed();

    int runtimed = m_runtime % 100;
    if (runtimed > 0 && runtimed < 15) {
        int min = m_runtime/1000/60;
        int sec = m_runtime/1000 - min*60;
        int ms  = m_runtime - sec*1000;
        //std::cout << "Current time: " << min << ":" << sec << ":" << ms << std::endl;

        /*
        // testing decrementHp()
        if (sec % 2 == 1) {
            decrementHp();
        }
        */
    }

    // REMOVED: Don't auto-reset HP when it reaches 0
    // Game Over should be handled by GameView
    /*
    if (m_playerHp <= 0)
    {
        resetGameTime();
    }
    */
}

void StateController::resetGameTime()
{
    m_runtime   = 0;
    m_isRunning = false;

    m_playerHp = MAX_HEARTS;

    resetHeartPosition();
}

void StateController::decrementHp(int number)
{
    m_playerHp -= number;
    
    // Guard against negative HP
    if(m_playerHp < 0)
    {
        m_playerHp = 0;
        return;
    }
    
    if (m_playerHp >= 0 && m_playerHp < MAX_HEARTS && m_hearts[m_playerHp])
    {
        // set heart image out of bounds
        Vector v = Vector(-m_heartWidth*2, -10);
        m_hearts[m_playerHp]->setPosition(v);
    }
}

void StateController::render()
{
}

StateController::~StateController()
{
    if (m_timer)
    {
        delete m_timer;
    }

//    if (m_hearts)
//    {
        for (int i; i < MAX_HEARTS; i++)
        {
            if (m_hearts[i]) {
                delete m_hearts[i];
            }
        }
        
//        delete m_hearts;
//    }

/*
    if (m_heartTexture)
    {
        delete m_heartTexture;
    }
*/
}

} /* namespace jumper */

