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
 * @brief Monster (Ghost, Snake). Snake patrouilliert; Ghost verfolgt den Actor 5 Sek. und kann springen.
 */
class Monster : public MovingRenderable
{
public:
    enum class Type { Ghost, Snake };

    Monster(MainWindow* mw, SDL_Texture* tilesetTexture,
            Type type, double x, double y,
            double leftBound, double rightBound,
            int tileWidth, int tileHeight, int tilesPerRow, int tileOffset);

    /// Update: Snake patrouilliert; Ghost verfolgt Actor (wenn nicht null) 5 Sek., kann springen
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
    /// Nur Ghost: Chase-Timer (Sekunden), nach 5 s zurück zu Patrouille
    double      m_chaseTimer;
};

} // namespace jumper

#endif // MONSTER_HPP
