/*
 *  Monster.cpp
 */

#include "game/include/Monster.hpp"
#include "game/include/MainWindow.hpp"
#include "game/include/Actor.hpp"

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
    , m_groundY(0.0)
    , m_chaseTimer(0.0)
    , m_exhaustedTimer(0.0)
{
    // Ghost: 131 Top, 132 Bottom | Snake: 133 Top, 134 Bottom (0-basiert: 131/132, 133/134)
    int tileTop = (type == Type::Ghost) ? 131 : 133;
    int tileBottom = (type == Type::Ghost) ? 132 : 134;
    setTileSourceRect(m_sourceRectTop, tileTop, tileWidth, tileHeight);
    setTileSourceRect(m_sourceRectBottom, tileBottom, tileWidth, tileHeight);
    m_targetRect.w = tileWidth;
    m_targetRect.h = tileHeight * 2;

    setWorldPosition(Vector<double>(x, y));
    setVelocity(Vector<double>(m_moveSpeed, 0));

    if (m_type == Type::Ghost)
    {
        m_groundY = y + tileHeight * 2;
        m_chaseTimer = 5.0;  // Jagd 5 Sekunden ab Spawn
    }
}

void Monster::update(double dt, Actor* actor)
{
    if (m_type == Type::Snake)
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
        return;
    }

    const double chaseSpeed = 75.0;
    const double chaseDuration = 5.0;
    const double exhaustedDuration = 5.0;
    const double jumpSpeed = -280.0;
    const double gravity = 400.0;
    const double actorAboveThreshold = 20.0;

    Vector<double> pos = worldPosition();
    Vector<double> vel = velocity();
    bool onGround = (pos.y() + h() >= m_groundY - 1.0);

    // Erschöpft: 5 s nur Patrouille, dann wieder Jagd
    if (m_exhaustedTimer > 0.0)
    {
        m_exhaustedTimer -= dt;
        if (m_exhaustedTimer <= 0.0)
            m_chaseTimer = chaseDuration;
        vel.setX((vel.x() > 0) ? m_moveSpeed : -m_moveSpeed);
        vel.setY(0.0);
    }
    // Jagd: 5 s verfolgen, dann erschöpft
    else if (actor && m_chaseTimer > 0.0)
    {
        double ax = actor->worldPosition().x() + actor->w() / 2.0;
        double ay = actor->worldPosition().y() + actor->h() / 2.0;
        double mx = pos.x() + w() / 2.0;
        double my = pos.y() + h() / 2.0;

        double dx = ax - mx;
        double dy = ay - my;
        double dist = std::sqrt(dx * dx + dy * dy);

        if (dist > 1.0)
        {
            vel.setX((dx / dist) * chaseSpeed);
            vel.setY((dy / dist) * chaseSpeed * 0.9);

            if (onGround && dy < -actorAboveThreshold)
                vel.setY(jumpSpeed);
        }
        m_chaseTimer -= dt;
        if (m_chaseTimer <= 0.0)
            m_exhaustedTimer = exhaustedDuration;
    }
    else
    {
        vel.setX((vel.x() > 0) ? m_moveSpeed : -m_moveSpeed);
        vel.setY(0.0);
    }

    vel.setY(vel.y() + gravity * dt);
    pos.setX(pos.x() + vel.x() * dt);
    pos.setY(pos.y() + vel.y() * dt);

    if (!actor || vel.y() >= 0)
    {
        if (pos.y() + h() >= m_groundY)
        {
            pos.setY(m_groundY - h());
            vel.setY(0.0);
        }
    }

    if (!actor)
    {
        if (pos.x() <= m_leftBound)
        {
            pos.setX(m_leftBound);
            vel.setX(std::abs(vel.x()));
        }
        else if (pos.x() + w() >= m_rightBound)
        {
            pos.setX(m_rightBound - w());
            vel.setX(-std::abs(vel.x()));
        }
    }

    setWorldPosition(pos);
    setVelocity(vel);
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
