/*
    *  Physics.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "game/include/Physics.hpp"
#include "game/include/Actor.hpp"
#include "game/include/Level.hpp"

#include <SDL.h>
#include <cmath>

#include <iostream>
using namespace std;

namespace jumper {



Physics::Physics(Actor* actor,
                 Level* level)
    : m_actor(actor), m_level(level)
{
    m_tiles = m_level->tiles();
    m_lastTicks = SDL_GetTicks();
    m_lastHazardDamageTicks = 0;
    m_pendingKnockback = Vector2f(0, 0);
    m_knockbackFramesRemaining = 0;
    m_pendingMovement = Vector2f(0, 0);
}

namespace {
    /// Prüft ob die Tile-ID zu einem Hazard-Typ gehört (RulesTiles.xml)
    bool isHazardTile(int tileId)
    {
        switch (tileId)
        {
            case 119:  // Box Explosive
            case 127:  // Spikes
            case 144:  // Ghost White Top
            case 145:  // Ghost White Bottom
            case 146:  // Snake Slime Top
            case 147:  // Snake Slime Bottom
                return true;
            default:
                return false;
        }
    }
}

void Physics::update()
{
    unsigned int currentTicks = SDL_GetTicks();

    // Get elapsed time since last call (in seconds)
    double dt = (currentTicks - m_lastTicks) / 1000.0;

    // Update velocity und berechne Bewegung
    updateActorPosition(dt);

    // Substepping: Bewegung in kleine Schritte aufteilen, nach jedem Schritt Kollision prüfen
    // Verhindert Durchdringen von Wänden bei hoher Geschwindigkeit
    const double maxStepSize = 10.0;
    const int maxSubsteps = 20;
    Vector2f remaining = m_pendingMovement;
    int substepCount = 0;
    while((std::abs(remaining.x()) > 0.1 || std::abs(remaining.y()) > 0.1) && substepCount < maxSubsteps)
    {
        double len = std::sqrt(remaining.x() * remaining.x() + remaining.y() * remaining.y());
        Vector2f step;
        if(len <= maxStepSize)
        {
            step = remaining;
            remaining = Vector2f(0, 0);
        }
        else
        {
            double scale = maxStepSize / len;
            step = Vector2f(remaining.x() * scale, remaining.y() * scale);
            remaining.setX(remaining.x() - step.x());
            remaining.setY(remaining.y() - step.y());
        }
        m_actor->setWorldPosition(m_actor->worldPosition() + step);
        resolveCollision();
        substepCount++;
    }
    m_pendingMovement = Vector2f(0, 0);
    
    // FINALE ABSOLUTE PRÜFUNG: Stelle sicher, dass die Position nach allen Updates korrekt ist
    // Diese Prüfung wird IMMER ausgeführt, auch wenn die Position bereits über der Grenze ist
    if(m_level && m_actor)
    {
        const Camera& camera = m_level->getCamera();
        int cameraX = camera.x();
        int cameraWidth = camera.width();
        int cameraRight = cameraX + cameraWidth;
        Vector2f currentPos = m_actor->worldPosition();
        int actorWidth = m_actor->w();
        
        // DEBUG: Ausgabe der Werte
        if(currentPos.x() + actorWidth > cameraRight)
        {
            std::cout << "DEBUG: Camera X: " << cameraX << ", Width: " << cameraWidth << ", Right: " << cameraRight << std::endl;
            std::cout << "DEBUG: Actor Pos X: " << currentPos.x() << ", Width: " << actorWidth << ", Right Edge: " << (currentPos.x() + actorWidth) << std::endl;
            std::cout << "DEBUG: CORRECTING: Actor right edge " << (currentPos.x() + actorWidth) << " exceeds camera right " << cameraRight << std::endl;
            m_actor->setWorldPosition(Vector2f(cameraRight - actorWidth, currentPos.y()));
            m_actor->velocity().setX(0);
        }
        
        // Prüfe linke Grenze
        if(currentPos.x() < 0)
        {
            m_actor->setWorldPosition(Vector2f(0, currentPos.y()));
            m_actor->velocity().setX(0);
        }
    }

    // Update clock
    m_lastTicks = SDL_GetTicks();
}

void Physics::updateActorPosition(double dt)
{
    if(dt > 0 && m_actor)
    {
        // Test if a new jump was initiated. Test if actor is on
        // ground. If on ground, set jump start flag to false
        // and indicate, that the player is jumping. This is
        // needed when integrating the jump force
        if(m_actor->wantsToJump() && m_actor->onGround())
        {
            m_actor->setJumping(true);
            m_actor->setWantsToJump(false);
        }

        Vector2f d_gravity;
        Vector2f d_move;

        // Compute the velocity differeces that come from gravity
        // and move force
        d_gravity = m_level->forces().gravity() * dt;
        // Während Knockback: Gravitation stark reduzieren, damit Charakter diagonal nach oben fliegen kann
        if(m_knockbackFramesRemaining > 0)
            d_gravity = Vector2f(d_gravity.x() * 0.15, d_gravity.y() * 0.15);
        d_move = m_actor->forces().moveForce() * dt;
        // Während Knockback: Lenken stark reduzieren (kein Turbo-Boost in der Luft)
        if(m_knockbackFramesRemaining > 0)
            d_move = Vector2f(d_move.x() * 0.03, d_move.y() * 0.03);

        // Update velocity
        m_actor->setVelocity(m_actor->velocity() + d_move + d_gravity);

        // Add jumping momentum
        if(m_actor->jumping())
        {
            m_actor->velocity().setY(
                        m_actor->velocity().y() + (m_actor->forces().jumpForce().y() * dt));
        }

        // Damp velocity - während Knockback sanfter, damit Flug länger dauert
        const double dampBase = m_level->forces().damping().x();
        const double dampDuringKnockback = 0.94;  // höher = langsamer Ausklang, längere Flugdauer
        double damp = (m_knockbackFramesRemaining > 0) ? dampDuringKnockback : dampBase;
        m_actor->setVelocity(m_actor->velocity() * damp);

        // Knockback portionenweise anwenden (jeden Frame ein Stück = langsamere Beschleunigung)
        const double knockbackApplyFraction = 0.15;  // Anteil pro Frame (niedriger = sanfter, langsamer)
        if(m_pendingKnockback.x() != 0 || m_pendingKnockback.y() != 0)
        {
            double applyX = m_pendingKnockback.x() * knockbackApplyFraction;
            double applyY = m_pendingKnockback.y() * knockbackApplyFraction;
            m_actor->setVelocity(m_actor->velocity() + Vector2f(applyX, applyY));
            m_pendingKnockback.setX(m_pendingKnockback.x() - applyX);
            m_pendingKnockback.setY(m_pendingKnockback.y() - applyY);
            if(std::abs(m_pendingKnockback.x()) < 0.5 && std::abs(m_pendingKnockback.y()) < 0.5)
                m_pendingKnockback = Vector2f(0, 0);
            m_knockbackFramesRemaining = 25;
        }

        // Clamp velocities (überspringen während Knockback, damit Geschwindigkeit natürlich ausklingt)
        if(m_knockbackFramesRemaining <= 0)
        {
            if(m_actor->velocity().x() > m_actor->forces().maxRunVelocity() * dt)
            {
                m_actor->setVelocity(Vector2f(m_actor->forces().maxRunVelocity() * dt,
                        m_actor->velocity().y()));
            }

            if(m_actor->velocity().x() < -m_actor->forces().maxRunVelocity() * dt)
            {
                m_actor->setVelocity(Vector2f(-m_actor->forces().maxRunVelocity() * dt,
                        m_actor->velocity().y()));
            }

            if(m_actor->velocity().y() > m_actor->forces().maxFallVelocity() * dt)
            {
               m_actor->setVelocity(
                        Vector2f(m_actor->velocity().x(), m_actor->forces().maxFallVelocity() * dt));
            }

            if(m_actor->velocity().y() < -m_actor->forces().maxJumpVelocity() * dt)
            {
                m_actor->setVelocity(
                        Vector2f(m_actor->velocity().x(), -m_actor->forces().maxJumpVelocity() * dt));
            }
        }
        else
        {
            m_knockbackFramesRemaining--;
        }

        //cout << "V: " << m_actor->velocity() << endl;

        // Bewegung pro Frame begrenzen in der Luft (verhindert Durchdringen von Wänden beim Lenken)
        Vector2f velForPosition = m_actor->velocity();
        bool inAir = !m_actor->onGround();
        if(inAir)
        {
            const double maxMovePerFrame = 28.0;  // ~1 Tile, verhindert Tunneling
            double speed = std::sqrt(velForPosition.x() * velForPosition.x() + velForPosition.y() * velForPosition.y());
            if(speed > maxMovePerFrame && speed > 0.01)
            {
                double scale = maxMovePerFrame / speed;
                velForPosition = Vector2f(velForPosition.x() * scale, velForPosition.y() * scale);
                m_actor->setVelocity(velForPosition);
            }
        }

        // Prüfe Kamera-Grenzen BEVOR die Position gesetzt wird
        // Verhindere, dass die Position über die Grenze hinausgeht
        if(m_level)
        {
            const Camera& camera = m_level->getCamera();
            // Kamera X-Position (400) + Kamera-Breite (Fenster-Breite) = rechte Grenze in World-Koordinaten
            // Die Kamera-Breite ist die Fenster-Breite, da die Kamera mit mainWindow->w() initialisiert wird
            int cameraRight = camera.x() + camera.width();
            Vector2f currentPos = m_actor->worldPosition();
            Vector2f newPosition = currentPos + velForPosition;
            
            // Prüfe rechte Grenze: Stoppe Geschwindigkeit, wenn Position über Grenze hinausgehen würde
            // WICHTIG: Prüfe auch die aktuelle Position, falls sie bereits über der Grenze ist
            int actorRightEdge = newPosition.x() + m_actor->w();
            if(actorRightEdge > cameraRight)
            {
                // Setze Position auf Maximum und stoppe Geschwindigkeit
                newPosition.setX(cameraRight - m_actor->w());
                m_actor->velocity().setX(0);
            }
            
            // Zusätzliche Prüfung: Falls aktuelle Position bereits über Grenze ist, korrigiere sofort
            int currentRightEdge = currentPos.x() + m_actor->w();
            if(currentRightEdge > cameraRight)
            {
                newPosition.setX(cameraRight - m_actor->w());
                m_actor->velocity().setX(0);
            }
            
            // Prüfe linke Grenze
            if(newPosition.x() < 0)
            {
                newPosition.setX(0);
                m_actor->velocity().setX(0);
            }
            
            // Zusätzliche Prüfung: Falls aktuelle Position bereits unter 0 ist, korrigiere sofort
            if(currentPos.x() < 0)
            {
                newPosition.setX(0);
                m_actor->velocity().setX(0);
            }
            
            // Bewegung speichern für Substepping (wird in update() angewendet)
            m_pendingMovement = newPosition - currentPos;
        }
        else
        {
            m_pendingMovement = velForPosition;
        }


        // Stop jumping at maximum jumping height
        if(fabs(m_actor->worldPosition().y() - m_actor->jumpStart()) >= m_actor->forces().maxJumpHeight())
        {
            m_actor->setJumping(false);
        }

        //Collision c = level.resolveCollision(this);
        //cout << c.delta() << endl;
    }
}

void Physics::getSurroundingTiles(const Vector2f& pos, Vector<int>* tiles)
{
    /* Determine x and y position of the sprite within the grid */
    // Berücksichtige den Y-Offset der Tiles (600), damit die Kollisionserkennung korrekt funktioniert
    // Die Tiles werden mit Y-Offset 600 gerendert, also müssen wir das bei der Kollisionserkennung berücksichtigen
    Vector<int> gridPos(
                floor((pos.x() + 0.5 * m_actor->w()) /  m_tiles->tileWidth()),
                floor((pos.y() - 600 + 0.5 * m_actor->h()) / m_tiles->tileHeight()));

    /* Get the surrounding tiles in "priority" order, i.e., we want
     * check some collisions like left before we check the others
     */
    tiles[0].setX(gridPos.x() - 1);
    tiles[0].setY(gridPos.y() - 1);

    tiles[1].setX(gridPos.x());
    tiles[1].setY(gridPos.y() - 1);

    tiles[2].setX(gridPos.x() + 1);
    tiles[2].setY(gridPos.y() - 1);

    tiles[3].setX(gridPos.x() - 1);
    tiles[3].setY(gridPos.y());

    tiles[4].setX(gridPos.x() + 1);
    tiles[4].setY(gridPos.y());

    tiles[5].setX(gridPos.x() - 1);
    tiles[5].setY(gridPos.y() + 1);

    tiles[6].setX(gridPos.x());
    tiles[6].setY(gridPos.y() + 1);

    tiles[7].setX(gridPos.x() + 1);
    tiles[7].setY(gridPos.y() + 1);

}

void Physics::resolveCollision()
{
    SDL_Rect tileRect;
    SDL_Rect spriteRect;
    SDL_Rect intersectionRect;
    Vector2f desiredPosition;
    Vector<int> surroundingTiles[8];


    //Convert the player sprite's screen position to global position
    //Vector2f global_pos = m_player->position() + Vector2f(m_camera.position().x(), m_camera.position().y());

    Vector2f global_pos = m_actor->worldPosition();

    // Set desired position to new position
    desiredPosition = global_pos;

    // Prüfe Kamera-Grenzen VOR den Tile-Kollisionen, um sicherzustellen, dass die Position nicht über die Grenze hinausgeht
    if(m_level)
    {
        const Camera& camera = m_level->getCamera();
        int cameraRight = camera.x() + camera.width();
        
        // Begrenze desiredPosition auf Kamera-Grenze (rechts)
        if(desiredPosition.x() + m_actor->w() > cameraRight)
        {
            desiredPosition.setX(cameraRight - m_actor->w());
            if(m_actor->velocity().x() > 0)
            {
                m_actor->velocity().setX(0);
            }
        }
        
        // Begrenze desiredPosition auf Level-Grenze (links)
        if(desiredPosition.x() < 0)
        {
            desiredPosition.setX(0);
            if(m_actor->velocity().x() < 0)
            {
                m_actor->velocity().setX(0);
            }
        }
    }

    // Check if sprite intersects with one of its surrounding tiles
    getSurroundingTiles(global_pos, surroundingTiles);

    int d_y, d_x;
    int f_y, f_x;
    m_actor->setOnGround(false);
    f_y = surroundingTiles[6].y();
    f_x = surroundingTiles[6].x();

    if(m_actor->velocity().x() > 0)
    {
        d_y = surroundingTiles[7].y();
        d_x = surroundingTiles[7].x();
    }
    else
    {
        d_y = surroundingTiles[5].y();
        d_x = surroundingTiles[5].x();
    }

    // Prüfe Grenzen für f_x und f_y (auch negative Werte)
    if(f_y >= 0 && f_y < m_tiles->height() && f_x >= 0 && f_x < m_tiles->width())
    {
        if(m_tiles->get(f_x, f_y) > 0)
        {
            m_actor->setOnGround(true);
        }
    }


    // Prüfe Grenzen für d_x und d_y (auch negative Werte)
    if(d_y >= 0 && d_y < m_tiles->height() && d_x >= 0 && d_x < m_tiles->width() )
    {
        if(m_tiles->get(d_x, d_y) > 0)
        {
            m_actor->setOnGround(true);
        }
    }

    for(int n = 0; n < 8; n++)
    {
        int x = surroundingTiles[n].x();
        int y = surroundingTiles[n].y();

        // Check, if tile coordinates are valid
        if((y >= 0) && (y < m_tiles->height()) && (x >= 0) && (x < m_tiles->width()) )
        {

            int tileId = m_tiles->get(x, y);
            if(tileId > 0)
            {

                // Get SDL rect for current tile and sprite and check intersection
                // Berücksichtige den Y-Offset der Tiles (600)
                tileRect.y = y * m_tiles->tileHeight() + 600;
                tileRect.x = x * m_tiles->tileWidth();
                tileRect.w = m_tiles->tileWidth();
                tileRect.h = m_tiles->tileHeight();

                spriteRect.x = desiredPosition.x();
                spriteRect.y = desiredPosition.y();
                spriteRect.w = m_actor->w();
                spriteRect.h = m_actor->h();


                if(SDL_IntersectRect(&tileRect, &spriteRect, &intersectionRect))
                {
                    // Hazard-Kollision: Schaden + "Aua!" + Wegschießen
                    if(isHazardTile(tileId) && m_level && m_level->getStateController())
                    {
                        unsigned int now = SDL_GetTicks();
                        const unsigned int hazardCooldownMs = 1000;
                        if(now - m_lastHazardDamageTicks >= hazardCooldownMs)
                        {
                            m_level->getStateController()->decrementHp(1);
                            std::cout << "Aua!" << std::endl;
                            m_lastHazardDamageTicks = now;

                            // Charakter diagonal nach oben wegschießen (via m_pendingKnockback)
                            double tileCenterX = tileRect.x + tileRect.w / 2.0;
                            double tileCenterY = tileRect.y + tileRect.h / 2.0;
                            double actorCenterX = spriteRect.x + spriteRect.w / 2.0;
                            double actorCenterY = spriteRect.y + spriteRect.h / 2.0;
                            double dx = actorCenterX - tileCenterX;  // Richtung weg vom Hazard
                            // Diagonal nach oben: Horizontal weg vom Hazard, Y stark nach oben
                            double dirX = (std::abs(dx) < 0.01) ? 0.5 : ((dx > 0) ? 0.5 : -0.5);
                            double dirY = -1.2;  // Starker Aufwärtsimpuls (Y negativ = oben)
                            double len = std::sqrt(dirX * dirX + dirY * dirY);
                            if(len > 0.01)
                            {
                                dirX /= len;
                                dirY /= len;
                                const double knockbackStrength = 50.0;
                                m_pendingKnockback = Vector2f(dirX * knockbackStrength, dirY * knockbackStrength);
                            }
                        }
                    }


                	if(n == 3)
                	{

                	}

                	if(n == 6)
                	{
                		m_actor->setOnGround(true);
                	}

                	// Handle pose correction cases
                	if(n == 4)
                	{
                		desiredPosition.setX(desiredPosition.x() - intersectionRect.w);
                		// Prüfe sofort nach X-Änderung: Linke Grenze
                		if(desiredPosition.x() < 0)
                		{
                			desiredPosition.setX(0);
                			m_actor->velocity().setX(0);
                		}
                	}
                	else if(n == 1)
                	{
                		desiredPosition.setY(desiredPosition.y() + intersectionRect.h);
                		m_actor->setJumping(false);
                	}
                	else if(n == 3)
                	{
                		desiredPosition.setX(desiredPosition.x() + intersectionRect.w);
                		// Prüfe sofort nach X-Änderung: Rechte Kamera-Grenze
                		if(m_level)
                		{
                			const Camera& camera = m_level->getCamera();
                			int cameraRight = camera.x() + camera.width();
                			if(desiredPosition.x() + m_actor->w() > cameraRight)
                			{
                				desiredPosition.setX(cameraRight - m_actor->w());
                				m_actor->velocity().setX(0);
                			}
                		}
                	}
                	else if(n == 6)
                	{
                		desiredPosition.setY(desiredPosition.y() - intersectionRect.h);
                	}
                	else
                	{
                		if(intersectionRect.w >= 2 && intersectionRect.h >= 2)
                		{
                			if(intersectionRect.w > intersectionRect.h)
                			{
                				if( (n == 5) || (n == 7))
                				{
                					desiredPosition.setY(desiredPosition.y() - intersectionRect.h);
                				}
                				else
                				{
                					desiredPosition.setY(desiredPosition.y() + intersectionRect.h);
                				}
                			}
                			else
                			{
                				if( (n == 2) || (n == 7))
                				{
                					desiredPosition.setX(desiredPosition.x() - intersectionRect.w);
                					// Prüfe sofort nach X-Änderung: Linke Grenze
                					if(desiredPosition.x() < 0)
                					{
                						desiredPosition.setX(0);
                						m_actor->velocity().setX(0);
                					}
                				}
                				else
                				{
                					desiredPosition.setX(desiredPosition.x() + intersectionRect.w);
                					// Prüfe sofort nach X-Änderung: Rechte Kamera-Grenze
                					if(m_level)
                					{
                						const Camera& camera = m_level->getCamera();
                						int cameraRight = camera.x() + camera.width();
                						if(desiredPosition.x() + m_actor->w() > cameraRight)
                						{
                							desiredPosition.setX(cameraRight - m_actor->w());
                							m_actor->velocity().setX(0);
                						}
                					}
                				}
                				if( (n == 0) || (n == 2) )
                				{
                					m_actor->setJumping(false);
                				}
                			}
                		}
                	}

                }
            }
        }
    }

    // Prüfe Level-Grenzen (links) und Kamera-Grenzen (rechts)
    // Verhindere, dass der Spieler außerhalb des Levels (links) oder der Kamera (rechts) läuft
    // WICHTIG: Diese Prüfung muss NACH allen Tile-Kollisionen sein, um sicherzustellen, dass die Position korrekt ist
    if(m_level)
    {
        // Hole Kamera vom Level für rechte Grenze
        const Camera& camera = m_level->getCamera();
        
        // Berechne die Grenzen in Welt-Koordinaten
        // Linke Grenze: Level-Start (0)
        // Rechte Grenze: Kamera X-Position + Kamera Breite
        int levelLeft = 0;
        int cameraRight = camera.x() + camera.width();
        
        // Prüfe linke Grenze (Level-Grenze)
        if(desiredPosition.x() < levelLeft)
        {
            desiredPosition.setX(levelLeft);
            // Stoppe horizontale Geschwindigkeit, wenn gegen linke Wand
            if(m_actor->velocity().x() < 0)
            {
                m_actor->velocity().setX(0);
            }
        }
        
        // Prüfe rechte Grenze (Kamera-Grenze, berücksichtige Sprite-Breite)
        // Die rechte Kante des Sprites (desiredPosition.x() + m_actor->w()) darf nicht über cameraRight hinausgehen
        if(desiredPosition.x() + m_actor->w() > cameraRight)
        {
            desiredPosition.setX(cameraRight - m_actor->w());
            // Stoppe horizontale Geschwindigkeit, wenn gegen rechte Wand
            m_actor->velocity().setX(0);
        }
        
        // Zusätzliche Sicherheitsprüfung: Stelle sicher, dass die Position nicht negativ wird
        if(desiredPosition.x() < 0)
        {
            desiredPosition.setX(0);
            m_actor->velocity().setX(0);
        }
        
        // FINALE PRÜFUNG: Stelle absolut sicher, dass die Position innerhalb der Grenzen ist
        // Diese Prüfung wird direkt vor setWorldPosition ausgeführt
        int finalCameraRight = camera.x() + camera.width();
        if(desiredPosition.x() + m_actor->w() > finalCameraRight)
        {
            desiredPosition.setX(finalCameraRight - m_actor->w());
            m_actor->velocity().setX(0);
        }
        if(desiredPosition.x() < 0)
        {
            desiredPosition.setX(0);
            m_actor->velocity().setX(0);
        }
    }

    // FINALE PRÜFUNG direkt vor setWorldPosition
    if(m_level && m_actor)
    {
        const Camera& camera = m_level->getCamera();
        int cameraRight = camera.x() + camera.width();
        
        // Stelle absolut sicher, dass die Position innerhalb der Grenzen ist
        if(desiredPosition.x() + m_actor->w() > cameraRight)
        {
            desiredPosition.setX(cameraRight - m_actor->w());
            m_actor->velocity().setX(0);
            std::cout << "FINAL CHECK: Correcting position to " << desiredPosition.x() << " (max: " << (cameraRight - m_actor->w()) << ")" << std::endl;
        }
        if(desiredPosition.x() < 0)
        {
            desiredPosition.setX(0);
            m_actor->velocity().setX(0);
        }
    }
    
    m_actor->setWorldPosition(desiredPosition);
    
    // ABSOLUT LETZTE PRÜFUNG direkt nach setWorldPosition
    if(m_level && m_actor)
    {
        const Camera& camera = m_level->getCamera();
        int cameraRight = camera.x() + camera.width();
        Vector2f posAfterSet = m_actor->worldPosition();
        
        if(posAfterSet.x() + m_actor->w() > cameraRight)
        {
            std::cout << "AFTER SET: Position still exceeds! Pos: " << posAfterSet.x() << ", Right: " << (posAfterSet.x() + m_actor->w()) << ", Max: " << cameraRight << std::endl;
            m_actor->setWorldPosition(Vector2f(cameraRight - m_actor->w(), posAfterSet.y()));
            m_actor->velocity().setX(0);
        }
    }
}

} // namespace jumper
