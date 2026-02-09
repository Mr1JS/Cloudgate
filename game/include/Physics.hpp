/*
 *  Physics.hpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 *
 *  Modifiziert für Box2D-Integration.
 */

#ifndef PHYSICS_H
#define PHYSICS_H

#include "TileSetRepresentation.hpp"
#include "ActorForces.hpp"
#include "Util.hpp"
#include "Vector.hpp"

#include <map>
#include <vector>
#include <box2d/box2d.h>

namespace jumper {

class Actor;
class Level;
class Physics;

/**
 * @brief Contact-Listener
 */
class ContactListener : public b2ContactListener
{
public:
    ContactListener(Actor* actor, Level* level, Physics* physics);

    void BeginContact(b2Contact* contact) override;

private:
    Actor*      m_actor;
    Level*      m_level;
    Physics*    m_physics;
};

/**
 * @brief Box2D-basierte Physik-Engine.
 *        Berechnet Kollisionen und Bewegung über Box2D.
 */
class Physics
{
public:
	/// Konstruktor mit Actor und Level
    Physics(Actor* actor, Level* level);

    /// Destruktor
    ~Physics();

    /// Führt einen Physik-Update-Schritt aus
    void update();

    /// Wird vom ContactListener bei Hazard-Kollision aufgerufen
    void handleHazardContact(int tileId, const b2Vec2& tileCenter, const b2Vec2& actorCenter);

    /// Wendet Knockback an (z.B. bei Monster-Kollision). otherCenter in Pixel-Koordinaten.
    void applyKnockbackFromPosition(const Vector2f& otherCenter);

    b2Body* getActorBody() const { return m_actorBody; }
    unsigned int getLastHazardDamageTicks() const { return m_lastHazardDamageTicks; }
    void setLastHazardDamageTicks(unsigned int t) { m_lastHazardDamageTicks = t; }
    // get Tile Data in ContactListener
    std::pair<std::string, std::string> getTileData(int tileId);

    /// Body nach Kontakt-Callback zerstören (z.B. gesammelte Münze)
    void queueBodyForDestruction(b2Body* body);

    /// Feder/Spring (z.B. Tile 120): Actor mit factor-facher Sprungkraft nach oben schleudern
    void applySpringLaunch(float factor);

private:

    /// Erstellt statische Box2D-Bodies aus den Level-Tiles
    void buildLevelBodies();

    /// Konvertiert Pixel-Position zu Box2D-Koordinaten (Meter, Y invertiert)
    b2Vec2 toBox2D(const Vector2f& pixel) const;

    /// Konvertiert Box2D-Position zurück zu Pixel
    Vector2f fromBox2D(const b2Vec2& world) const;

    /// Wendet Spielersteuerung auf den Box2D-Body an
    void applyPlayerInput(double dt);

    /// Prüft und korrigiert Kamera-Grenzen
    void enforceCameraBounds();

    /// Der Actor im Level
    Actor*                  m_actor;

    /// Tile-Set für Kollisionsgeometrie
    TileSetRepresentation*  m_tiles;

    /// Das Level
    Level*                  m_level;

    /// Box2D-Welt
    b2World*                m_world;

    /// Box2D-Body des Actors
    b2Body*                 m_actorBody;

    /// Contact-Listener (Ownership)
    ContactListener*        m_contactListener;

    /// Letzter Ticks-Wert für Delta-Zeit
    unsigned int            m_lastTicks;

    /// Zeitpunkt des letzten Hazard-Schadens (Invincibility-Frames)
    unsigned int            m_lastHazardDamageTicks;

    /// Skalierung Pixel pro Meter für Box2D
    static constexpr float  PIXELS_PER_METER = 32.0f;

    /// Y-Offset der Tile-Welt in Pixel
    static constexpr float  TILE_Y_OFFSET = 600.0f;

    /// Bodies, die nach dem Kontakt-Callback zerstört werden (z.B. Collectibles)
    std::vector<b2Body*> m_bodiesToDestroy;
};

} // namespace jumper

#endif // PHYSICS_H
