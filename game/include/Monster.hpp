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

class MainWindow;

/**
 * @brief Monster (Ghost, Snake) das auf dem Level patrouilliert
 */
class Monster : public MovingRenderable
{
public:
    enum class Type { Ghost, Snake };

    Monster(MainWindow* mw, SDL_Texture* tilesetTexture,
            Type type, double x, double y,
            double leftBound, double rightBound,
            int tileWidth, int tileHeight, int tilesPerRow, int tileOffset);

    /// Update: Bewegung links/rechts
    void update(double dt);

    virtual void render() override;

    Type type() const { return m_type; }

private:
    Type        m_type;
    double      m_moveSpeed;
    double      m_leftBound;
    double      m_rightBound;
    SDL_Rect    m_sourceRectTop;     // Top-Tile (Ghost Top / Snake Top)
    SDL_Rect    m_sourceRectBottom;  // Bottom-Tile (Ghost Bottom / Snake Bottom)
};

} // namespace jumper

#endif // MONSTER_HPP
