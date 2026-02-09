/*
 *  Monster.hpp
 *  Feind (Ghost, Snake) mit Patrouillen-Bewegung
 */

#ifndef MONSTER_HPP
#define MONSTER_HPP

#include "MovingRenderable.hpp"
#include "Vector.hpp"

#include <SDL.h>

namespace jumper {

class Actor;
class MainWindow;

/**
 * @brief Monster (Ghost, Snake). Snake patrouilliert; Ghost jagt 5 s, ist 5 s erschöpft, dann wieder Jagd.
 */
class Monster : public MovingRenderable
{
public:
    enum class Type { Ghost, Snake };

    Monster(MainWindow* mw, SDL_Texture* tilesetTexture,
            Type type, double x, double y,
            double leftBound, double rightBound,
            int tileWidth, int tileHeight, int tilesPerRow, int tileOffset);

    /// Update: Snake patrouilliert; Ghost jagt 5 s / erschöpft 5 s im Wechsel
    void update(double dt, Actor* actor = nullptr);

    virtual void render() override;

    Type type() const { return m_type; }

private:
    Type        m_type;
    double      m_moveSpeed;
    double      m_leftBound;
    double      m_rightBound;
    SDL_Rect    m_sourceRectTop;
    SDL_Rect    m_sourceRectBottom;

    /// Nur Ghost: Boden-Y (Füße) bei Patrouille
    double      m_groundY;
    /// Nur Ghost: Chase-Timer (Sekunden), 5 s verfolgen
    double      m_chaseTimer;
    /// Nur Ghost: Erschöpfungs-Timer (Sekunden), 5 s Pause nach der Jagd
    double      m_exhaustedTimer;
};

} // namespace jumper

#endif // MONSTER_HPP
