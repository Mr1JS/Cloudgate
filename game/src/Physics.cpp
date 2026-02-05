/*
 *  Physics.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 *
 *  Box2D-Integration.
 */

#include "game/include/Physics.hpp"
#include "game/include/Actor.hpp"
#include "game/include/Level.hpp"
#include "game/include/Util.hpp"

#include <SDL.h>
#include <cmath>
#include <iostream>

namespace jumper {

// --- ContactListener ---

ContactListener::ContactListener(Actor* actor, Level* level, Physics* physics)
    : m_actor(actor), m_level(level), m_physics(physics)
{
}

void ContactListener::BeginContact(b2Contact* contact)
{
    b2Fixture* fa = contact->GetFixtureA();
    b2Fixture* fb = contact->GetFixtureB();
    b2Body* bodyA = fa->GetBody();
    b2Body* bodyB = fb->GetBody();

    // Eines der Bodies ist der Actor, das andere ein Tile
    b2Body* actorBody = m_physics->getActorBody();
    b2Fixture* tileFixture = nullptr;
    b2Body* tileBody = nullptr;

    if (bodyA == actorBody)
    {
        tileFixture = fb;
        tileBody = bodyB;
    }
    else if (bodyB == actorBody)
    {
        tileFixture = fa;
        tileBody = bodyA;
    }
    else
    {
        return;
    }
    int tileId = static_cast<int>(tileFixture->GetUserData().pointer);

    // invalid tileID
    if (tileId < 0)
    {
        return;
    }
    std::string tileType = m_physics->getTileData(tileId).second;

    // Collectible (Münze etc.): von Karte entfernen, Münzzähler erhöhen, Body später zerstören
    if (tileType == "collectible")
    {
        uintptr_t posData = tileBody->GetUserData().pointer;
        int gx = static_cast<int>((posData >> 16) & 0xFFFFu);
        int gy = static_cast<int>(posData & 0xFFFFu);
        if (m_level)
        {
            m_level->removeTileAt(gx, gy);
            if (m_level->getStateController())
            {
                m_level->getStateController()->addCoin();
            }
        }
        m_physics->queueBodyForDestruction(tileBody);
        return;
    }
    
    // take dmg (monster and traps)
    if (tileType == "hazard" || tileType == "enemy")
    {
        unsigned int now = SDL_GetTicks();
        const unsigned int hazardCooldownMs = 1000;
        if (now - m_physics->getLastHazardDamageTicks() < hazardCooldownMs)
        {
            return;
        }

        m_physics->setLastHazardDamageTicks(now);
        if (m_level && m_level->getStateController())
        {
            m_level->getStateController()->decrementHp(1);
            std::cout << "Aua!" << std::endl;
        }

        b2Vec2 actorCenter = actorBody->GetWorldCenter();
        b2Vec2 tileCenter = tileBody->GetWorldCenter();
        m_physics->handleHazardContact(tileId, tileCenter, actorCenter);
    }

    // check win condition and end game if condition is met
    if (tileType == "door")
    {
        if (m_level->checkAndUpdateGoalState() == GOALSTATE_WINNABLE)
        {
            std::cout << "Dobby is free!" << std::endl;
            m_level->win();
        }
    }
}

// --- Physics ---

Physics::Physics(Actor* actor, Level* level)
    : m_actor(actor), m_level(level)
{
    m_tiles = m_level->tiles();
    m_lastTicks = SDL_GetTicks();
    m_lastHazardDamageTicks = 0;
    m_contactListener = nullptr;

    if (!m_tiles || !m_actor)
    {
        m_world = nullptr;
        m_actorBody = nullptr;
        return;
    }
    // Tile-Formen aus RulesTiles.xml (für Halb- und Diagonal-Tiles)
    std::string resPath = m_level->getResPath();
    if (!resPath.empty() && resPath.back() != '/' && resPath.back() != '\\')
        resPath += "/";
    std::string rulesPath = resPath + "tileDefinition/RulesTiles.xml";
    m_tileData = jumper::ParseXMLData(rulesPath);
    if (m_tileData.empty())
    {
        m_tileData = jumper::ParseXMLData("res/tileDefinition/RulesTiles.xml");
    }
    std::cout << "[Physics] Tile shapes: " << m_tileData.size()
              << " (" << (m_tileData.empty() ? "using full boxes" : rulesPath) << ")" << std::endl;

    // Box2D-Gravitation: Level hat gravity_y=400 (nach unten), Box2D Y+ ist oben
    const LevelForces& lf = m_level->forces();
    b2Vec2 gravity(0.0f, -lf.gravity().y() / PIXELS_PER_METER);
    m_world = new b2World(gravity);

    // Actor-Body erstellen (Box2D nutzt Körpermitte, Actor nutzt linke obere Ecke)
    Vector2f topLeft = m_actor->worldPosition();
    float centerX = topLeft.x() + m_actor->w() / 2.0f;
    float centerY = topLeft.y() + m_actor->h() / 2.0f;
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = toBox2D(Vector2f(centerX, centerY));
    bodyDef.fixedRotation = true;
    bodyDef.linearDamping = 1.0f - static_cast<float>(lf.damping().x());
    m_actorBody = m_world->CreateBody(&bodyDef);

    // Etwas kleinere Kollisionsbox verhindert Hängenbleiben an Kanten
    b2PolygonShape box;
    float shrink = 0.85f;
    float hw = (m_actor->w() / 2.0f) * shrink / PIXELS_PER_METER;
    float hh = (m_actor->h() / 2.0f) * shrink / PIXELS_PER_METER;
    box.SetAsBox(hw, hh);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &box;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.05f;
    fixtureDef.restitution = 0.0f;
    m_actorBody->CreateFixture(&fixtureDef);

    // Level-Geometrie aus Tiles
    buildLevelBodies();

    // Contact-Listener
    m_contactListener = new ContactListener(m_actor, m_level, this);
    m_world->SetContactListener(m_contactListener);
}

Physics::~Physics()
{
    if (m_world)
    {
        m_world->SetContactListener(nullptr);
        delete m_contactListener;
        delete m_world;
        m_world = nullptr;
    }
    m_actorBody = nullptr;
}

b2Vec2 Physics::toBox2D(const Vector2f& pixel) const
{
    return b2Vec2(
        pixel.x() / PIXELS_PER_METER,
        -(pixel.y() - TILE_Y_OFFSET) / PIXELS_PER_METER
    );
}

Vector2f Physics::fromBox2D(const b2Vec2& world) const
{
    return Vector2f(
        world.x * PIXELS_PER_METER,
        TILE_Y_OFFSET - world.y * PIXELS_PER_METER
    );
}

void Physics::buildLevelBodies()
{
    if (!m_tiles || !m_world)
    {
        return;
    }

    int tw = m_tiles->tileWidth();
    int th = m_tiles->tileHeight();
    int levelW = m_tiles->width();
    int levelH = m_tiles->height();

    const float hwM = (tw / 2.0f) / PIXELS_PER_METER;
    const float hhM = (th / 2.0f) / PIXELS_PER_METER;

    for (int gy = 0; gy < levelH; ++gy)
    {
        for (int gx = 0; gx < levelW; ++gx)
        {
            int tileId = m_tiles->get(gx, gy) - 1;
            if (tileId < 0)
            {
                continue;
            }

            float px = gx * tw;
            float py = gy * th + TILE_Y_OFFSET;

            std::string shapeType = "full";
            auto it = m_tileData.find(tileId);
            if (it != m_tileData.end())
                shapeType = it->second.shape;

            b2Vec2 center;
            b2PolygonShape shape;
            b2FixtureDef fixtureDef;
            fixtureDef.friction = 0.05f;
            fixtureDef.userData.pointer = static_cast<uintptr_t>(tileId);

            if (shapeType == "half_bottom")
            {
                center = toBox2D(Vector2f(px + tw/2.0f, py + th/2.0f + th/4.0f));
                float hw = (tw / 2.0f) / PIXELS_PER_METER;
                float hh = (th / 4.0f) / PIXELS_PER_METER;
                shape.SetAsBox(hw, hh);
            }
            else if (shapeType == "half_top")
            {
                center = toBox2D(Vector2f(px + tw/2.0f, py + th/4.0f));
                float hw = (tw / 2.0f) / PIXELS_PER_METER;
                float hh = (th / 4.0f) / PIXELS_PER_METER;
                shape.SetAsBox(hw, hh);
            }
            else if (shapeType == "half_left")
            {
                center = toBox2D(Vector2f(px + tw/4.0f, py + th/2.0f));
                float hw = (tw / 4.0f) / PIXELS_PER_METER;
                float hh = (th / 2.0f) / PIXELS_PER_METER;
                shape.SetAsBox(hw, hh);
            }
            else if (shapeType == "half_right")
            {
                center = toBox2D(Vector2f(px + 3.0f*tw/4.0f, py + th/2.0f));
                float hw = (tw / 4.0f) / PIXELS_PER_METER;
                float hh = (th / 2.0f) / PIXELS_PER_METER;
                shape.SetAsBox(hw, hh);
            }
            else if (shapeType == "diag_tl_br" || shapeType == "diag_tr_bl")
            {
                center = toBox2D(Vector2f(px + tw/2.0f, py + th/2.0f));
                b2Vec2 verts[3];
                if (shapeType == "diag_tl_br")
                {
                    // Slope \ (Linie oben-links → unten-rechts): begehbar = Dreieck unten-links
                    verts[0].Set(-hwM, -hhM);
                    verts[1].Set(-hwM,  hhM);
                    verts[2].Set( hwM, -hhM);
                }
                else
                {
                    // diag_tr_bl: Slope / (Linie oben-rechts → unten-links): begehbar = Dreieck unten-rechts
                    verts[0].Set( hwM, -hhM);
                    verts[1].Set( hwM,  hhM);
                    verts[2].Set(-hwM, -hhM);
                }
                shape.Set(verts, 3);
            }
            else
            {
                // full (Standard)
                center = toBox2D(Vector2f(px + tw/2.0f, py + th/2.0f));
                shape.SetAsBox(hwM, hhM);
            }

            fixtureDef.shape = &shape;
            b2BodyDef bodyDef;
            bodyDef.type = b2_staticBody;
            bodyDef.position = center;
            bodyDef.userData.pointer = (static_cast<uintptr_t>(gx) << 16) | (static_cast<uintptr_t>(gy) & 0xFFFFu);
            b2Body* body = m_world->CreateBody(&bodyDef);
            body->CreateFixture(&fixtureDef);
        }
    }
}

void Physics::queueBodyForDestruction(b2Body* body)
{
    if (body)
        m_bodiesToDestroy.push_back(body);
}

void Physics::update()
{
    if (!m_world || !m_actorBody || !m_actor)
    {
        return;
    }

    // Bodies zerstören, die im Kontakt-Callback (z.B. Collectibles) markiert wurden
    for (b2Body* body : m_bodiesToDestroy)
        m_world->DestroyBody(body);
    m_bodiesToDestroy.clear();

    unsigned int currentTicks = SDL_GetTicks();
    double dt = (currentTicks - m_lastTicks) / 1000.0;
    if (dt <= 0)
    {
        dt = 1.0 / 60.0;
    }
    if (dt > 0.1)
    {
        dt = 0.1;
    }

    // Spielereingabe anwenden (vor Step)
    applyPlayerInput(dt);

    // Box2D-Step
    const float timeStep = static_cast<float>(dt);
    const int32 velocityIterations = 8;
    const int32 positionIterations = 3;
    m_world->Step(timeStep, velocityIterations, positionIterations);

    // Position und Velocity zurück zum Actor (Box2D-Zentrum -> linke obere Ecke)
    b2Vec2 centerBox2D = m_actorBody->GetPosition();
    Vector2f center = fromBox2D(centerBox2D);
    Vector2f topLeft(center.x() - m_actor->w() / 2.0f, center.y() - m_actor->h() / 2.0f);
    b2Vec2 vel = m_actorBody->GetLinearVelocity();
    m_actor->setWorldPosition(topLeft);
    m_actor->velocity().setX(vel.x * PIXELS_PER_METER);
    m_actor->velocity().setY(-vel.y * PIXELS_PER_METER);

    // onGround per Raycast
    struct GroundRayCallback : b2RayCastCallback
    {
        bool hit = false;
        float ReportFixture(b2Fixture* fixture, const b2Vec2&, const b2Vec2&, float) override
        {
            if (fixture->GetBody()->GetType() == b2_staticBody)
            {
                hit = true;
            }
            return 0;
        }
    };
    b2Vec2 rayStart = m_actorBody->GetWorldCenter();
    b2Vec2 rayEnd = rayStart + b2Vec2(0, -((m_actor->h() / 2.0f) / PIXELS_PER_METER + 0.05f));
    GroundRayCallback cb;
    m_world->RayCast(&cb, rayStart, rayEnd);
    bool onGround = cb.hit;
    m_actor->setOnGround(onGround);

    if (onGround)
    {
        m_actor->setJumping(false);
    }

    enforceCameraBounds();

    m_lastTicks = SDL_GetTicks();
}

void Physics::applyPlayerInput(double dt)
{
    if (!m_actorBody || !m_actor) return;

    // Horizontale Bewegung
    float moveX = m_actor->forces().moveForce().x() * static_cast<float>(dt) / PIXELS_PER_METER * 10.5f;
    b2Vec2 vel = m_actorBody->GetLinearVelocity();
    vel.x += moveX;

    float maxRun = m_actor->forces().maxRunVelocity() / PIXELS_PER_METER;
    vel.x = b2Clamp(vel.x, -maxRun, maxRun);
    m_actorBody->SetLinearVelocity(vel);

    // Sprung (Impuls statt Kraft über Zeit - äquivalent zur ursprünglichen Sprung-Logik)
    if (m_actor->wantsToJump() && m_actor->onGround())
    {
        m_actor->setWantsToJump(false);
        m_actor->setJumping(true);
        float jumpImpulse = -m_actor->forces().jumpForce().y() / PIXELS_PER_METER * 2.2f;
        m_actorBody->ApplyLinearImpulseToCenter(b2Vec2(0, jumpImpulse), true);
    }

    // Max Fall/Jump Velocity (in m/s) – maxJump muss hoch genug sein, sonst wird Sprung abgewürgt
    vel = m_actorBody->GetLinearVelocity();
    float maxFall = m_actor->forces().maxFallVelocity() / PIXELS_PER_METER;
    float maxJump = static_cast<float>(m_actor->forces().maxJumpVelocity()) / PIXELS_PER_METER;
    if (maxJump < 13.0f) maxJump = 13.0f;
    vel.y = b2Clamp(vel.y, -maxFall, maxJump);
    m_actorBody->SetLinearVelocity(vel);
}

void Physics::enforceCameraBounds()
{
    if (!m_level || !m_actor) return;

    const Camera& camera = m_level->getCamera();
    int cameraRight = camera.x() + camera.width();
    Vector2f pos = m_actor->worldPosition();

    if (pos.x() + m_actor->w() > cameraRight)
    {
        m_actor->setWorldPosition(Vector2f(cameraRight - m_actor->w(), pos.y()));
        m_actor->velocity().setX(0);
        if (m_actorBody)
            m_actorBody->SetLinearVelocity(b2Vec2(0, m_actorBody->GetLinearVelocity().y));
    }
    if (pos.x() < 0)
    {
        m_actor->setWorldPosition(Vector2f(0, pos.y()));
        m_actor->velocity().setX(0);
        if (m_actorBody)
            m_actorBody->SetLinearVelocity(b2Vec2(0, m_actorBody->GetLinearVelocity().y));
    }

    if (m_actorBody)
    {
        Vector2f topLeft = m_actor->worldPosition();
        float cx = topLeft.x() + m_actor->w() / 2.0f;
        float cy = topLeft.y() + m_actor->h() / 2.0f;
        m_actorBody->SetTransform(toBox2D(Vector2f(cx, cy)), 0);
    }
}

void Physics::handleHazardContact(int, const b2Vec2& tileCenter, const b2Vec2& actorCenter)
{
    if (!m_actorBody) return;

    // Diagonal nach oben wegschleudern, horizontal weg von der Falle
    float dx = actorCenter.x - tileCenter.x;
    float dirX = (std::abs(dx) < 0.01f) ? 0.0f : ((dx > 0) ? 1.0f : -1.0f);
    float dirY = 1.0f;  // Box2D: Y+ = oben
    float len = std::sqrt(dirX*dirX + dirY*dirY);
    dirX /= len;
    dirY /= len;
    float strength = 280.0f / PIXELS_PER_METER;
    m_actorBody->ApplyLinearImpulseToCenter(b2Vec2(dirX * strength, dirY * strength), true);
}

void Physics::applyKnockbackFromPosition(const Vector2f& otherCenter)
{
    if (!m_actorBody || !m_actor) return;

    Vector2f actorCenter(m_actor->worldPosition().x() + m_actor->w() / 2.0f,
                         m_actor->worldPosition().y() + m_actor->h() / 2.0f);
    b2Vec2 actorBox = toBox2D(actorCenter);
    b2Vec2 otherBox = toBox2D(otherCenter);

    float dx = actorBox.x - otherBox.x;
    float dirX = (std::abs(dx) < 0.01f) ? 0.0f : ((dx > 0) ? 1.0f : -1.0f);
    float dirY = 1.0f;
    float len = std::sqrt(dirX*dirX + dirY*dirY);
    dirX /= len;
    dirY /= len;
    float strength = 280.0f / PIXELS_PER_METER;
    m_actorBody->ApplyLinearImpulseToCenter(b2Vec2(dirX * strength, dirY * strength), true);
}

std::pair<std::string, std::string> Physics::getTileData(int tileId)
{
    const TileInfo& t = m_tileData[tileId];
    return { t.name, t.type };
}


} // namespace jumper
