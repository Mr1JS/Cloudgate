/**
 * @file Monster.hpp
 * @brief Defines the Monster class for enemy entities (Ghost and Snake) with AI behavior
 */

/*
 *  Monster.hpp
 *  Feind (Ghost, Snake) mit Patrouillen-Bewegung
 */

#ifndef MONSTER_HPP
#define MONSTER_HPP

#include "MovingRenderable.hpp"
#include "Vector.hpp"

#include <SDL.h>

namespace jumper
{

class Actor;
class MainWindow;

/**
 * @brief Monster (Ghost, Snake). Snake patrols; Ghost chases player on sight for 5s, then has a delay of 5s before chasing again.
 */
class Monster : public MovingRenderable
{
public:

    // enum for monster type classification
    enum class Type { Ghost, Snake };

    Monster(MainWindow* mw, SDL_Texture* tilesetTexture,
            Type type, double x, double y,
            double leftBound, double rightBound,
            int tileWidth, int tileHeight, int tilesPerRow, int tileOffset);

    /// Behavior logic: Snake patrols; Ghost chases on sight for 5s / then stops chasing for 5s in tandem
    void update(double dt, Actor* actor = nullptr);

    /// Render monster sprite to screen
    virtual void render() override;

    // return type of this monster
    Type type() const { return m_type; }

private:
    /// @brief this monster's type
    Type        m_type;

    /// movement speed
    double      m_moveSpeed;

    /// left bound (hitbox)
    double      m_leftBound;

    /// right bound (hitbox)
    double      m_rightBound;

    /// source rect of top sprite
    SDL_Rect    m_sourceRectTop;

    /// source rect of bottom sprite
    SDL_Rect    m_sourceRectBottom;

    /// Ghost only: Ground-Y for patrol
    double      m_groundY;

    /// Ghost only: Chase-timer (in seconds), default of 5s
    double      m_chaseTimer;
    
    /// Ghost only: fatigue-timer (in seconds), default of 5s
    double      m_exhaustedTimer;
};

} // namespace jumper

#endif // MONSTER_HPP
