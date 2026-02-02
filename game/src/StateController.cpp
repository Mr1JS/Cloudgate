#include "game/include/StateController.hpp"

#include <fstream>
#include "game/include/Util.hpp"

namespace jumper
{

StateController::StateController(MainWindow* mainWindow, std::string& filename)
: m_hearts{}, m_runtimeDigits{}
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
            Vector v = Vector(m_mainWindow->w() - m_heartWidth * (i + 1), 10);
            m_hearts[i]->setPosition(v);
        }
    }
}

std::array<TimerDigit*, RUNTIME_DIGITS> StateController::addDigits(SDL_Texture* digitTexture, int numFrames, int frameWidth, int frameHeight, int layer)
{
    int x = 25;
    for (int i = 0; i < RUNTIME_DIGITS; i++)
    {
        m_runtimeDigits[i] = new TimerDigit(m_mainWindow, digitTexture, numFrames, frameWidth, frameHeight);
        
        m_runtimeDigits[i]->setWorldPosition(Vector2f(x, 25));
        
        x += 5 + frameWidth;

        if (i == 1 || i == 3)
        {
            x += 10;
        }
    }

    return m_runtimeDigits;
}

bool StateController::isPaused()
{
    return !m_isRunning;
}

void StateController::startGame()
{
    m_isRunning = true;
    m_timer->start();
}

void StateController::updateGameTime()
{
    if (isPaused())
    {
        return;
    }

    unsigned int runtimed = m_timer->elapsed();
    if (runtimed - m_runtime > 100) {
        unsigned int min = m_runtime/1000/60;
        unsigned int sec = m_runtime/1000 - min*60;
        unsigned int ms  = m_runtime - sec*1000;
        std::cout << "Current time: " << min << ":" << sec << ":" << ms << std::endl;
        updateRuntimeGraphics(runtimed);
        m_runtime = runtimed;
    }
}

void StateController::stop()
{
    m_isRunning = false;
}

void StateController::updateRuntimeGraphics(unsigned int runtime)
{
    // TODO: this would need to be changed if RUNTIME_DIGITS < 6
    unsigned int min = m_runtime/1000/60;
    unsigned int sec = m_runtime/1000 - min*60;
    unsigned int ms  = m_runtime - sec*1000;
    unsigned int digit = 0;
    for (int i = RUNTIME_DIGITS; i > 0; i--)
    {
        if (i > RUNTIME_DIGITS-3)
        {
            digit = ms % 10;
            ms /= 10;
        }
        else if (i > RUNTIME_DIGITS-5)
        {
            digit = sec % 10;
            sec /= 10;
        }
        else
        {
            digit = min % 10;
            min /= 10;
        }
        m_runtimeDigits[i]->setValue(digit);
    }
    */
}

void StateController::resetGame()
{
    m_runtime   = 0;
    m_isRunning = false;

    m_playerHp = MAX_HEARTS;

    resetHeartPosition();
}

int StateController::getHp()
{
    return m_playerHp;
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

