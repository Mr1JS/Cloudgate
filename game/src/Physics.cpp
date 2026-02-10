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
#include "game/include/Monster.hpp"
#include "game/include/Util.hpp"

#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
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

    if (tileId < 0)
    {
        return;
    }
    std::string tileType = m_physics->getTileData(tileId).second;

    // Wandkontakt für feste Tiles (verhindert Hängen/Zittern an Wänden)
    if (tileType != "collectible" && tileType != "red_potion" && tileType != "blue_potion" && tileType != "random")
    {
        b2WorldManifold wm;
        contact->GetWorldManifold(&wm);
        float nx = (bodyA == actorBody) ? wm.normal.x : -wm.normal.x;
        m_physics->addWallContact(nx);
    }

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

    // Red Potion: ein Herz hinzufügen, Tile entfernen, Body zerstören
    if (tileType == "red_potion")
    {
        uintptr_t posData = tileBody->GetUserData().pointer;
        int gx = static_cast<int>((posData >> 16) & 0xFFFFu);
        int gy = static_cast<int>(posData & 0xFFFFu);
        if (m_level)
        {
            m_level->removeTileAt(gx, gy);
            if (m_level->getStateController())
            {
                m_level->getStateController()->incrementHp(1);
            }
        }
        m_physics->queueBodyForDestruction(tileBody);
        return;
    }

    // Blue Potion (Super-Trank): Blinken, schneller, höher springen, 10 Sekunden unverwundbar
    if (tileType == "blue_potion")
    {
        uintptr_t posData = tileBody->GetUserData().pointer;
        int gx = static_cast<int>((posData >> 16) & 0xFFFFu);
        int gy = static_cast<int>(posData & 0xFFFFu);
        if (m_level)
        {
            m_level->removeTileAt(gx, gy);
            if (m_level->getStateController())
            {
                m_level->getStateController()->activateSuperPotion();
            }
        }
        m_physics->queueBodyForDestruction(tileBody);
        return;
    }

    // Random-Box (id 108): Box verschwindet, zufällig erscheint Sprungbrett, Trank, Falle (hazard) oder Monster (enemy).
    // Geist und Schlange: 2 Tiles (Top oben, Bottom unten). Spawn so, dass der BODEN (Bottom) auf der Box-Zelle steht (gy),
    // Top eine Zeile darüber (gy-1) – dann steht das Monster auf dem „Button“, nicht mit dem Kopf in der Erde.
    if (tileType == "random")
    {
        uintptr_t posData = tileBody->GetUserData().pointer;
        int gx = static_cast<int>((posData >> 16) & 0xFFFFu);
        int gy = static_cast<int>(posData & 0xFFFFu);
        std::vector<int> options;
        int jumpdownId = m_physics->getTileIdByType("jumpdown");
        int redPotionId = m_physics->getTileIdByType("red_potion");
        int bluePotionId = m_physics->getTileIdByType("blue_potion");
        if (jumpdownId >= 0)
        {
            options.push_back(jumpdownId);
        }
        if (redPotionId >= 0)
        {
            options.push_back(redPotionId);
        }
        if (bluePotionId >= 0)
        {
            options.push_back(bluePotionId);
        }
        std::vector<int> hazardIds = m_physics->getTileIdsByType("hazard");
        for (int id : hazardIds) options.push_back(id);
        bool canPlaceTwoTile = (gy >= 1);  // Platz für Top in gy-1, Bottom auf Box-Zelle gy
        if (canPlaceTwoTile)
        {
            options.push_back(131);  // Ghost White Top (131) + Bottom (132)
            options.push_back(133);  // Snake Slime Top (133) + Bottom (134)
        }
        if (!options.empty() && m_level)
        {
            int chosenId = options[static_cast<size_t>(std::rand()) % options.size()];
            m_physics->queueBodyForDestruction(tileBody);
            if (chosenId == 131)
            {
                m_level->spawnMonsterAt(gx, gy, Monster::Type::Ghost);
            }
            else if (chosenId == 133)
            {
                m_level->spawnMonsterAt(gx, gy, Monster::Type::Snake);
            }
            else
            {
                m_level->setTileAt(gx, gy, chosenId + 1);
                m_physics->queueCreateBodyForTile(gx, gy);
            }
        }
        return;
    }

    // take dmg (monster and traps) – nur wenn nicht unverwundbar
    if (tileType == "hazard" || tileType == "enemy")
    {
        if (!m_physics->canTakeDamage())
        {
            return;
        }

        m_physics->setLastHazardDamageTicks(SDL_GetTicks());
        if (m_level && m_level->getStateController())
        {
            m_level->getStateController()->decrementHp(1);
            std::cout << "Aua!" << std::endl;
        }

        b2Vec2 actorCenter = actorBody->GetWorldCenter();
        b2Vec2 tileCenter = tileBody->GetWorldCenter();
        m_physics->handleHazardContact(tileId, tileCenter, actorCenter);
    }

    // Spring: "jumpdown" = Feder aktiv (schleudert), "jumpup" = bereits benutzt (einmal nutzbar)
    if (tileType == "jumpup" || tileType == "jumpdown")
    {
        uintptr_t posData = tileBody->GetUserData().pointer;
        int gx = static_cast<int>((posData >> 16) & 0xFFFFu);
        int gy = static_cast<int>(posData & 0xFFFFu);
        int currentValue = m_level ? m_level->getTileAt(gx, gy) : 0;
        std::string currentType;
        int currentTileId = -1;
        if (currentValue > 0)
        {
            currentTileId = currentValue - 1;  // Level 1-basiert → 0-basierte Tile-ID
            currentType = m_physics->getTileData(currentTileId).second;
            if (currentType != "jumpup" && currentType != "jumpdown")
            {
                currentTileId = currentValue;
                currentType = m_physics->getTileData(currentTileId).second;
            }
        }
        std::cout << "[Spring] Kollision tileId=" << tileId << " type=" << tileType
                  << " gx=" << gx << " gy=" << gy << " currentValue=" << currentValue
                  << " currentTileId=" << currentTileId << " currentType=\"" << currentType << "\"" << std::endl;
        if (currentType == "jumpdown")
        {
            std::cout << "[Spring] Launch nach oben (2x), setze Tile auf jumpup (benutzt)" << std::endl;
            m_physics->applySpringLaunch(2.0f);
            if (m_level)
            {
                int jumpupId = m_physics->getTileIdByType("jumpup");
                if (jumpupId >= 0)
                {
                    m_level->setTileAt(gx, gy, jumpupId + 1);  // Level speichert 1-basiert
                }
            }
        }
        else
        {
            std::cout << "[Spring] Bereits jumpup (benutzt) – kein Launch" << std::endl;
        }
        return;
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

void ContactListener::EndContact(b2Contact* contact)
{
    b2Body* bodyA = contact->GetFixtureA()->GetBody();
    b2Body* bodyB = contact->GetFixtureB()->GetBody();
    b2Body* actorBody = m_physics->getActorBody();
    if (bodyA != actorBody && bodyB != actorBody)
    {
        return;
    }
    b2WorldManifold wm;
    contact->GetWorldManifold(&wm);
    float nx = (bodyA == actorBody) ? wm.normal.x : -wm.normal.x;
    m_physics->removeWallContact(nx);
}

// --- Physics ---

Physics::Physics(Actor* actor, Level* level)
    : m_actor(actor), m_level(level)
{
    m_tiles = m_level->tiles();
    m_lastTicks = SDL_GetTicks();
    m_cameraStartTicks = m_lastTicks;
    m_lastHazardDamageTicks = 0;
    m_invincibleUntilTicks = 0;
    m_movementLockedUntilTicks = 0;
    m_wallContactLeft = 0;
    m_wallContactRight = 0;
    m_coyoteTimeLeft = 0.0f;
    m_contactListener = nullptr;

    if (!m_tiles || !m_actor)
    {
        m_world = nullptr;
        m_actorBody = nullptr;
        return;
    }
    
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

    // Kreis: Radius = halbe Höhe, damit Kreis-Unterkante = Füße (Charakter nicht im Boden)
    b2CircleShape circle;
    float shrink = 0.9f;
    float radiusPx = (m_actor->h() / 2.0f) * shrink;
    circle.m_radius = radiusPx / PIXELS_PER_METER;
    circle.m_p.Set(0.0f, 0.0f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.05f;   // Niedrig, damit Actor nicht an Tile-Kanten hängen bleibt
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
            auto it = m_level->tileData()->find(tileId);
            if (it != m_level->tileData()->end())
            {
                shapeType = it->second.shape;
            }

            b2Vec2 center;
            b2PolygonShape shape;
            b2FixtureDef fixtureDef;
            fixtureDef.friction = 0.05f;   // Keine Reibung an Tiles → Actor bleibt nicht an Kanten hängen
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
    {
        m_bodiesToDestroy.push_back(body);
    }
}

void Physics::queueCreateBodyForTile(int gx, int gy)
{
    m_tilesToCreateBodyFor.push_back({ gx, gy });
}

void Physics::createBodyForTile(int gx, int gy)
{
    if (!m_tiles || !m_world)
    {
        return;
    }
    int tw = m_tiles->tileWidth();
    int th = m_tiles->tileHeight();
    int levelW = m_tiles->width();
    int levelH = m_tiles->height();
    if (gx < 0 || gx >= levelW || gy < 0 || gy >= levelH)
    {
        return;
    }
    int tileId = m_tiles->get(gx, gy) - 1;
    if (tileId < 0)
    {
        return;
    }

    const float hwM = (tw / 2.0f) / PIXELS_PER_METER;
    const float hhM = (th / 2.0f) / PIXELS_PER_METER;
    float px = gx * tw;
    float py = gy * th + TILE_Y_OFFSET;

    std::string shapeType = "full";
    auto it = m_level->tileData()->find(tileId);
    if (it != m_level->tileData()->end())
    {
        shapeType = it->second.shape;
    }

    b2Vec2 center;
    b2PolygonShape shape;
    b2FixtureDef fixtureDef;
    fixtureDef.friction = 0.0f;
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
            verts[0].Set(-hwM, -hhM);
            verts[1].Set(-hwM,  hhM);
            verts[2].Set( hwM, -hhM);
        }
        else
        {
            verts[0].Set( hwM, -hhM);
            verts[1].Set( hwM,  hhM);
            verts[2].Set(-hwM, -hhM);
        }
        shape.Set(verts, 3);
    }
    else
    {
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

void Physics::update()
{
    if (!m_world || !m_actorBody || !m_actor)
    {
        return;
    }

    // Bodies zerstören, die im Kontakt-Callback (z.B. Collectibles) markiert wurden
    for (b2Body* body : m_bodiesToDestroy)
    {
        m_world->DestroyBody(body);
    }
    m_bodiesToDestroy.clear();

    // Neue Bodies für ersetzte Tiles anlegen (z.B. random-Box → jumpdown/red_potion)
    for (const auto& p : m_tilesToCreateBodyFor)
    {
        createBodyForTile(p.first, p.second);
    }
    m_tilesToCreateBodyFor.clear();

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

    // onGround per Raycast VOR applyPlayerInput, damit Sprung/Lenkung aktuellen Bodenstand nutzt
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
    b2Vec2 rayEnd = rayStart + b2Vec2(0, -((m_actor->h() / 2.0f) / PIXELS_PER_METER + 0.08f));
    GroundRayCallback cb;
    m_world->RayCast(&cb, rayStart, rayEnd);
    bool onGround = cb.hit;
    if (!onGround && m_actor->onGround())
    {
        m_coyoteTimeLeft = 0.12f;  // Coyote-Time nach Verlassen des Bodens
    }
    if (onGround)
    {
        m_coyoteTimeLeft = 0.0f;
    }

    m_actor->setOnGround(onGround);

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

    // Coyote-Time ablaufen lassen
    m_coyoteTimeLeft -= static_cast<float>(dt);
    if (m_coyoteTimeLeft < 0.0f)
    {
        m_coyoteTimeLeft = 0.0f;
    }

    if (onGround)
    {
        m_actor->setJumping(false);
        // Steht still auf Boden → Wand-Zähler zurücksetzen (verhindert dauerhafte Bewegungssperre)
        if (std::abs(vel.x) < 0.01f)
        {
            m_wallContactLeft = 0;
            m_wallContactRight = 0;
        }
    }

    // Wand-Kontakte auch zurücksetzen, wenn Bewegungssperre endet (nach Schaden)
    // Verhindert, dass Spieler nach Knockback gegen Wand hängen bleibt
    if (SDL_GetTicks() >= m_movementLockedUntilTicks && (m_wallContactLeft > 0 || m_wallContactRight > 0))
    {
        // Wenn Geschwindigkeit in entgegengesetzter Richtung → Wand-Kontakt war falsch positiv
        if ((m_wallContactLeft > 0 && vel.x > 0.1f) || (m_wallContactRight > 0 && vel.x < -0.1f))
        {
            m_wallContactLeft = 0;
            m_wallContactRight = 0;
        }
    }

    enforceCameraBounds();

    m_lastTicks = SDL_GetTicks();
}

bool Physics::canTakeDamage() const
{
    if (SDL_GetTicks() < m_invincibleUntilTicks)
    {
        return false;
    }
    // Super-Trank: 10 Sekunden Unverwundbarkeit
    if (m_level && m_level->getStateController() && m_level->getStateController()->isSuperPotionActive())
    {
        return false;
    }
    return true;
}

void Physics::setLastHazardDamageTicks(unsigned int t)
{
    m_lastHazardDamageTicks = t;
    m_invincibleUntilTicks = t + INVINCIBILITY_MS;
    m_movementLockedUntilTicks = t + MOVEMENT_LOCK_MS;
    // Wand-Kontakte nach Schaden zurücksetzen, damit Spieler nicht hängen bleibt
    m_wallContactLeft = 0;
    m_wallContactRight = 0;
}

void Physics::addWallContact(float normalX)
{
    const int maxWallContacts = 2;
    if (normalX > 0.35f && m_wallContactLeft < maxWallContacts)
    {
        ++m_wallContactLeft;
    }
    else if (normalX < -0.35f && m_wallContactRight < maxWallContacts)
    {
        ++m_wallContactRight;
    }
}

void Physics::removeWallContact(float normalX)
{
    if (normalX > 0.35f) {
        if (m_wallContactLeft > 0)
        {
            --m_wallContactLeft;
        }
    }
    else if (normalX < -0.35f) {
        if (m_wallContactRight > 0)
        {
            --m_wallContactRight;
        }
    }
}

void Physics::applyPlayerInput(double dt)
{
    if (!m_actorBody || !m_actor)
    {
        return;
    }

    if (SDL_GetTicks() < m_movementLockedUntilTicks)
    {
        return;
    }

    float moveX = m_actor->forces().moveForce().x() * static_cast<float>(dt) / PIXELS_PER_METER * 10.5f;
    b2Vec2 vel = m_actorBody->GetLinearVelocity();
    vel.x += moveX;

    // Kein horizontaler Input + auf Boden → Bremsen (ersetzt physikalische Reibung bei friction=0)
    if (std::abs(moveX) < 0.0001f && m_actor->onGround())
    {
        vel.x *= 0.15f;
    }

    float maxRun = m_actor->forces().maxRunVelocity() / PIXELS_PER_METER;
    // Super-Trank: 1.5x Geschwindigkeit
    if (m_level && m_level->getStateController() && m_level->getStateController()->isSuperPotionActive())
        maxRun *= 1.5f;
    vel.x = b2Clamp(vel.x, -maxRun, maxRun);

    // Gegen Wand laufen unterbinden → verhindert Hängen und Zittern
    // Aber: Wenn Spieler sich aktiv wegbewegt (starker Input), Wand-Kontakt ignorieren
    // (verhindert Hängen nach Knockback)
    float inputStrength = std::abs(m_actor->forces().moveForce().x());
    if (m_wallContactLeft > 0 && vel.x < 0)
    {
        if (inputStrength > 50.0f && m_actor->forces().moveForce().x() > 0)
        {
            m_wallContactLeft = 0;  // Spieler drückt nach rechts → Wand-Kontakt links ignorieren
        }
        else
            vel.x = 0;
    }
    if (m_wallContactRight > 0 && vel.x > 0)
    {
        if (inputStrength > 50.0f && m_actor->forces().moveForce().x() < 0)
        {
            m_wallContactRight = 0;  // Spieler drückt nach links → Wand-Kontakt rechts ignorieren
        }
        else
            vel.x = 0;
    }

    m_actorBody->SetLinearVelocity(vel);

    // Sprung: erlaubt wenn onGround oder Coyote-Time (kurz nach Verlassen des Bodens)
    bool canJump = m_actor->onGround() || m_coyoteTimeLeft > 0.0f;
    if (m_actor->wantsToJump() && canJump)
    {
        m_actor->setWantsToJump(false);
        m_actor->setJumping(true);
        m_coyoteTimeLeft = 0.0f;
        float jumpImpulse = -m_actor->forces().jumpForce().y() / PIXELS_PER_METER * 2.2f;
        // Super-Trank: 1.5x Sprungkraft
        if (m_level && m_level->getStateController() && m_level->getStateController()->isSuperPotionActive())
        {
            jumpImpulse *= 1.5f;
        }
        m_actorBody->ApplyLinearImpulseToCenter(b2Vec2(0, jumpImpulse), true);
    }

    // Max Fall/Jump Velocity (in m/s) – maxJump muss hoch genug sein, sonst wird Sprung abgewürgt
    vel = m_actorBody->GetLinearVelocity();
    float maxFall = m_actor->forces().maxFallVelocity() / PIXELS_PER_METER;
    float maxJump = static_cast<float>(m_actor->forces().maxJumpVelocity()) / PIXELS_PER_METER;
    if (maxJump < 13.0f)
    {
        maxJump = 13.0f;
    }
    vel.y = b2Clamp(vel.y, -maxFall, maxJump);
    m_actorBody->SetLinearVelocity(vel);
}

void Physics::enforceCameraBounds()
{
    if (!m_level || !m_actor)
    {
        return;
    }

    const Camera& camera = m_level->getCamera();
    int cameraRight = camera.x() + camera.width();
    Vector2f pos = m_actor->worldPosition();

    if (pos.x() + m_actor->w() > cameraRight)
    {
        m_actor->setWorldPosition(Vector2f(cameraRight - m_actor->w(), pos.y()));
        m_actor->velocity().setX(0);
        if (m_actorBody)
        {
            m_actorBody->SetLinearVelocity(b2Vec2(0, m_actorBody->GetLinearVelocity().y));
        }
    }
    if (pos.x() < 0)
    {
        m_actor->setWorldPosition(Vector2f(0, pos.y()));
        m_actor->velocity().setX(0);
        if (m_actorBody)
        {
            m_actorBody->SetLinearVelocity(b2Vec2(0, m_actorBody->GetLinearVelocity().y));
        }
    }

    if (m_actorBody)
    {
        Vector2f topLeft = m_actor->worldPosition();
        float cx = topLeft.x() + m_actor->w() / 2.0f;
        float cy = topLeft.y() + m_actor->h() / 2.0f;
        m_actorBody->SetTransform(toBox2D(Vector2f(cx, cy)), 0);
    }
}

void Physics::applySpringLaunch(float factor)
{
    if (!m_actorBody || !m_actor)
    {
        return;
    }
    float jumpImpulse = -m_actor->forces().jumpForce().y() / PIXELS_PER_METER * 2.2f * factor;
    m_actorBody->ApplyLinearImpulseToCenter(b2Vec2(0, jumpImpulse), true);
    std::cout << "[Spring] applySpringLaunch factor=" << factor << " impulse=" << jumpImpulse << std::endl;
}

void Physics::handleHazardContact(int, const b2Vec2& tileCenter, const b2Vec2& actorCenter)
{
    if (!m_actorBody)
    {
        return;
    }

    // Knockback in Richtung weg vom Gegner/Falle (entgegengesetzte Richtung zur Kollision)
    float dx = actorCenter.x - tileCenter.x;
    float dy = actorCenter.y - tileCenter.y;
    float len = std::sqrt(dx*dx + dy*dy);
    if (len < 0.01f)
    {
        len = 1.0f;
    }
    float dirX = dx / len;
    float dirY = dy / len;
    float strength = (920.0f / 4.0f) / PIXELS_PER_METER;
    m_actorBody->ApplyLinearImpulseToCenter(b2Vec2(dirX * strength, dirY * strength), true);
}

void Physics::applyKnockbackFromPosition(const Vector2f& otherCenter)
{
    if (!m_actorBody || !m_actor)
    {
        return;
    }

    Vector2f actorCenter(m_actor->worldPosition().x() + m_actor->w() / 2.0f,
                         m_actor->worldPosition().y() + m_actor->h() / 2.0f);
    b2Vec2 actorBox = toBox2D(actorCenter);
    b2Vec2 otherBox = toBox2D(otherCenter);

    // Knockback weg vom Gegner (entgegengesetzte Richtung zur Kollision)
    float dx = actorBox.x - otherBox.x;
    float dy = actorBox.y - otherBox.y;
    float len = std::sqrt(dx*dx + dy*dy);
    if (len < 0.01f)
    {
        len = 1.0f;
    }
    float dirX = dx / len;
    float dirY = dy / len;
    float strength = (920.0f / 2.0f) / PIXELS_PER_METER;
    m_actorBody->ApplyLinearImpulseToCenter(b2Vec2(dirX * strength, dirY * strength), true);
}

std::pair<std::string, std::string> Physics::getTileData(int tileId)
{
    auto it = m_level->tileData()->find(tileId);
    if (it == m_level->tileData()->end())
    {
        return { "", "" };
    }
    const TileInfo& t = it->second;
    return { t.name, t.type };
}

int Physics::getTileIdByType(const std::string& type) const
{
    for (const auto& p : *(m_level->tileData()))
    {
        if (p.second.type == type)
        {
            return p.first;
        }
    }
    return -1;
}

std::vector<int> Physics::getTileIdsByType(const std::string& type) const
{
    std::vector<int> ids;
    for (const auto& p : *(m_level->tileData()))
    {
        if (p.second.type == type)
        {
            ids.push_back(p.first);
        }
    }
    return ids;
}

int Physics::getLevelWidth() const
{
    return m_tiles ? m_tiles->width() : 0;
}

int Physics::getLevelHeight() const
{
    return m_tiles ? m_tiles->height() : 0;
}

bool Physics::isCameraMovementEnabled() const
{
    return (SDL_GetTicks() - m_cameraStartTicks) >= CAMERA_DELAY_MS;
}

} // namespace jumper
