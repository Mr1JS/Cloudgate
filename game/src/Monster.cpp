/*
 *  Monster.cpp
 */

#include "game/include/Monster.hpp"
#include "game/include/MainWindow.hpp"

#include <algorithm>
#include <cmath>

namespace jumper {

namespace {
    // Formel wie in TileSet.cpp – dort werden 9 und 4 hardcodiert
    constexpr int TILES_PER_ROW = 9;
    constexpr int TILE_OFFSET = 4;

    void setTileSourceRect(SDL_Rect& src, int tileId, int tw, int th)
    {
        int row = tileId / TILES_PER_ROW;
        int col = tileId % TILES_PER_ROW;
        src.x = col * tw + TILE_OFFSET;
        if (col > 0) src.x += col * TILE_OFFSET;
        src.y = row * th + TILE_OFFSET;
        if (row > 0) src.y += row * TILE_OFFSET;
        src.w = tw;
        src.h = th;
    }
}

Monster::Monster(MainWindow* mw, SDL_Texture* tilesetTexture,
                 Type type, double x, double y,
                 double leftBound, double rightBound,
                 int tileWidth, int tileHeight, int /*tilesPerRow*/, int /*tileOffset*/)
    : MovingRenderable(mw, tilesetTexture)
    , m_type(type)
    , m_moveSpeed(40.0)
    , m_leftBound(leftBound)
    , m_rightBound(rightBound)
{
    // Ghost: 144 Top, 145 Bottom | Snake: 146 Top, 147 Bottom (0-basiert: 143/144, 145/146)
    int tileTop = (type == Type::Ghost) ? 144 : 146;
    int tileBottom = (type == Type::Ghost) ? 145 : 147;
    setTileSourceRect(m_sourceRectTop, tileTop, tileWidth, tileHeight);
    setTileSourceRect(m_sourceRectBottom, tileBottom, tileWidth, tileHeight);
    m_targetRect.w = tileWidth;
    m_targetRect.h = tileHeight * 2;

    setWorldPosition(Vector<double>(x, y));
    setVelocity(Vector<double>(m_moveSpeed, 0));
}

void Monster::update(double dt)
{
    Vector<double> pos = worldPosition();
    Vector<double> vel = velocity();

    pos.setX(pos.x() + vel.x() * dt);
    setWorldPosition(pos);

    if (pos.x() <= m_leftBound)
    {
        setVelocity(Vector<double>(m_moveSpeed, 0));
        setWorldPosition(Vector<double>(m_leftBound, pos.y()));
    }
    else if (pos.x() + w() >= m_rightBound)
    {
        setVelocity(Vector<double>(-m_moveSpeed, 0));
        setWorldPosition(Vector<double>(m_rightBound - w(), pos.y()));
    }
}

void Monster::render()
{
    if (!readyToRender()) return;

    Vector<int> pos = computeTargetPosition();
    SDL_RendererFlip flip = (velocity().x() < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    int th = m_targetRect.h / 2;

    SDL_Rect targetTop = { pos.x(), pos.y(), m_targetRect.w, th };
    SDL_RenderCopyEx(m_mainWindow->renderer(), m_texture, &m_sourceRectTop, &targetTop, 0, nullptr, flip);

    SDL_Rect targetBottom = { pos.x(), pos.y() + th, m_targetRect.w, th };
    SDL_RenderCopyEx(m_mainWindow->renderer(), m_texture, &m_sourceRectBottom, &targetBottom, 0, nullptr, flip);
}

} // namespace jumper
