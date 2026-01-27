#include "game/include/StateController.hpp"

#include <fstream>
#include "game/include/Util.hpp"

namespace jumper
{

StateController::StateController(MainWindow* mainWindow, std::string& filename)
{
    m_mainWindow = mainWindow;
    m_isRunning = false;
    m_runtime = 0;
    m_playerHp = MAX_HEARTS;
    m_timer = new QElapsedTimer();
    m_heartFileName = getHeartFileName(filename);
    m_heartTexture = LoadTexture(m_mainWindow->renderer(), m_heartFileName);
    m_hearts = new SDLRenderable[MAX_HEARTS];
    for (int i = 0; i < MAX_HEARTS; i++) 
    {
        m_hearts[i] = SDLRenderable(mainWindow, m_heartTexture);
        Vector v = Vector(m_mainWindow->w() - 32*i, 10);
        m_hearts[i].setPosition(v);
    }
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
        int ms  = m_runtime - sec*1000;
        std::cout << "Current time: " << min << ":" << sec << ":" << ms << std::endl;
    }
}

void StateController::resetGameTime()
{
    m_runtime   = 0;
    m_isRunning = false;

    // reset hp graphics
    for (int i = 0; i < MAX_HEARTS; i++) 
    {
        Vector v = Vector(m_mainWindow->w() - 32*i, 10);
        m_hearts[i].setPosition(v);
    }
}

void StateController::decrementHp(int number)
{
    m_playerHp -= number;
    // set heart image out of bounds
    Vector v = Vector(-32, -32);
    m_hearts[m_playerHp].setPosition(v);
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

    if (m_hearts)
    {
        /*
        for (int i; i < MAX_HEARTS; i++) {
            if (m_hearts[i]) {
                delete m_hearts[i];
            }
        }
        */
        delete m_hearts;
    }

    if (m_heartTexture)
    {
        delete m_heartTexture;
    }

}

} /* namespace jumper */

