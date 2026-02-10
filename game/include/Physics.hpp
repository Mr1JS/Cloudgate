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
#include <utility>
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
    void EndContact(b2Contact* contact) override;

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

    /// true = Spieler kann Schaden nehmen; false = noch unverwundbar
    bool canTakeDamage() const;

    /// Kamera darf sich erst nach CAMERA_DELAY_MS bewegen (z.B. 5 Sekunden nach Levelstart)
    bool isCameraMovementEnabled() const;
    void setLastHazardDamageTicks(unsigned int t);

    /// Bewegungssperre nach Schaden (ms)
    static constexpr unsigned int MOVEMENT_LOCK_MS = 250;
    /// Unverwundbarkeit nach Schaden (ms) - 2 Sekunden
    static constexpr unsigned int INVINCIBILITY_MS = 2000;

    // get Tile Data in ContactListener
    std::pair<std::string, std::string> getTileData(int tileId);

    /// Tile-ID (0-basiert) zu einem type aus RulesTiles.xml; -1 wenn nicht gefunden
    int getTileIdByType(const std::string& type) const;

    /// Alle Tile-IDs (0-basiert) mit gegebenem type (z.B. "enemy" für Monster)
    std::vector<int> getTileIdsByType(const std::string& type) const;

    /// Level-Ausdehnung in Tiles (für Prüfung z.B. gy+1 < getLevelHeight())
    int getLevelWidth() const;
    int getLevelHeight() const;

    /// Body nach Kontakt-Callback zerstören (z.B. gesammelte Münze)
    void queueBodyForDestruction(b2Body* body);

    /// Nach Ersetzen eines Tiles (z.B. random-Box): in nächstem update() Body für (gx, gy) anlegen
    void queueCreateBodyForTile(int gx, int gy);

    /// Feder/Spring (z.B. Tile 120): Actor mit factor-facher Sprungkraft nach oben schleudern
    void applySpringLaunch(float factor);

    /// Von ContactListener aufgerufen: Wandkontakt (verhindert Drücken in Wand → Zittern)
    void addWallContact(float normalX);
    void removeWallContact(float normalX);

private:

    /// Erstellt statische Box2D-Bodies aus den Level-Tiles
    void buildLevelBodies();

    /// Erstellt einen statischen Body für die Tile an (gx, gy); liest aktuellen Wert aus m_tiles
    void createBodyForTile(int gx, int gy);

    /// Konvertiert Pixel-Position zu Box2D-Koordinaten (Meter, Y invertiert)
    b2Vec2 toBox2D(const Vector2f& pixel) const;

    /// Konvertiert Box2D-Position zurück zu Pixel
    Vector2f fromBox2D(const b2Vec2& world) const;

    /// Wendet Spielersteuerung auf den Box2D-Body an
    void applyPlayerInput(double dt);

    /// Prüft und korrigiert Kamera-Grenzen
    void enforceCameraBounds();

    /// Geschlossene Türen öffnen (Nähe oder Coin-Ziel erfüllt)
    void updateDoors();

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

    /// Level-Start-Zeitpunkt (SDL_GetTicks), ab dem die Kamera nach CAMERA_DELAY_MS scrollen darf
    unsigned int            m_cameraStartTicks;

    /// Zeitpunkt des letzten Hazard-Schadens
    unsigned int            m_lastHazardDamageTicks;
    /// Bis zu diesem Zeitpunkt (SDL_GetTicks) ist der Spieler unverwundbar
    unsigned int            m_invincibleUntilTicks;

    /// Bis zu diesem Zeitpunkt (SDL_GetTicks) kann der Spieler sich nicht bewegen (nach Schaden)
    unsigned int            m_movementLockedUntilTicks;

    /// Wandkontakte: wenn > 0, darf sich der Actor nicht in diese Richtung bewegen (verhindert Hängen/Zittern)
    int                     m_wallContactLeft;
    int                     m_wallContactRight;

    /// Coyote-Time: Sprung noch möglich für kurze Zeit nach Verlassen des Bodens (Sekunden)
    float                   m_coyoteTimeLeft;

    /// Skalierung Pixel pro Meter für Box2D
    static constexpr float  PIXELS_PER_METER = 32.0f;

    /// Y-Offset der Tile-Welt in Pixel
    static constexpr float  TILE_Y_OFFSET = 600.0f;

    /// Verzögerung in ms, bis die Kamera sich zu bewegen beginnt (5 Sekunden)
    static constexpr unsigned int CAMERA_DELAY_MS = 5000;
    /// Tile-Definitionen (name, type, shape) aus RulesTiles.xml
    std::map<int, TileInfo> m_tileData;

    /// Bodies, die nach dem Kontakt-Callback zerstört werden (z.B. Collectibles)
    std::vector<b2Body*> m_bodiesToDestroy;

    /// (gx, gy) für die im nächsten update() ein neuer Body erzeugt werden soll (z.B. nach random-Box)
    std::vector<std::pair<int, int>> m_tilesToCreateBodyFor;
};

} // namespace jumper

#endif // PHYSICS_H
