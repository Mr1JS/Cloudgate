/*
    *  Physics.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "Physics.hpp"
#include "Actor.hpp"
#include "Level.hpp"

#include <SDL.h>

#include <iostream>
using namespace std;

namespace jumper {



Physics::Physics(Actor* actor,
                 Level* level)
    : m_actor(actor), m_level(level)
{
    m_tiles = m_level->tiles();
    m_lastTicks = SDL_GetTicks();
}

void Physics::update()
{
    unsigned int currentTicks = SDL_GetTicks();

    // Get elapsed time since last call (in seconds)
    double dt = (currentTicks - m_lastTicks) / 1000.0;

    // Update player position and resolve collisions
    updateActorPosition(dt);
    resolveCollision();

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
        d_move =    m_actor->forces().moveForce() * dt;

        // Update velocity
        m_actor->setVelocity(m_actor->velocity() + d_move + d_gravity);

        // Add jumping momentum
        if(m_actor->jumping())
        {
            m_actor->velocity().setY(
                        m_actor->velocity().y() + (m_actor->forces().jumpForce().y() * dt));
        }

        // Damp velocity according to extrinsic level damping
        m_actor->setVelocity(m_actor->velocity() * m_level->forces().damping().x());

        // Clamp velocities
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

        //cout << "V: " << m_actor->velocity() << endl;

        // Set new player position
        m_actor->setWorldPosition(m_actor->worldPosition() + m_actor->velocity());


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

            if(m_tiles->get(x, y) > 0)
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
//                    if(n == 6)
//                    {
//                        m_actor->setOnGround(true);
//                    }
//
//                    // Handle pose correction cases
//                    if(n == 4)
//                    {
//                        desiredPosition.setX(desiredPosition.x() - intersectionRect.w);
//                    }
//                    else if(n == 1)
//                    {
//                        desiredPosition.setY(desiredPosition.y() + intersectionRect.h);
//                        m_actor->setJumping(false);
//                    }
//                    else if(n == 3)
//                    {
//                        desiredPosition.setX(desiredPosition.x() + intersectionRect.w);
//                    }
//                    else if(n == 6)
//                    {
//                        desiredPosition.setY(desiredPosition.y() - intersectionRect.h);
//                    }
//                    else
//                    {
//                        if(intersectionRect.w >= 2 && intersectionRect.h >= 2)
//                        {
//                            if(intersectionRect.w > intersectionRect.h)
//                            {
//                                if( (n == 5) || (n == 7))
//                                {
//                                    desiredPosition.setY(desiredPosition.y() - intersectionRect.h);
//                                }
//                                else
//                                {
//                                    desiredPosition.setY(desiredPosition.y() + intersectionRect.h);
//                                }
//                            }
//                            else
//                            {
//                                if( (n == 2) || (n == 7))
//                                {
//                                    desiredPosition.setX(desiredPosition.x() - intersectionRect.w);
//                                }
//                                else
//                                {
//                                    desiredPosition.setX(desiredPosition.x() + intersectionRect.w);
//                                }
//                                if( (n == 0) || (n == 2) )
//                                {
//                                    m_actor->setJumping(false);
//                                }
//                            }
//                        }
//                    }
//
//                }

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
                	}
                	else if(n == 1)
                	{
                		desiredPosition.setY(desiredPosition.y() + intersectionRect.h);
                		m_actor->setJumping(false);
                	}
                	else if(n == 3)
                	{
                		desiredPosition.setX(desiredPosition.x() + intersectionRect.w);
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
                				}
                				else
                				{
                					desiredPosition.setX(desiredPosition.x() + intersectionRect.w);
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

    // Prüfe Level-Grenzen (links und rechts)
    // Verhindere, dass der Spieler außerhalb des Levels läuft
    if(m_tiles)
    {
        // Berechne die Level-Grenzen in Welt-Koordinaten
        // Level startet bei (0, 600) in Welt-Koordinaten (Y-Offset für weiter unten)
        int levelLeft = 0;
        int levelRight = m_tiles->width() * m_tiles->tileWidth();
        
        // Prüfe linke Grenze
        if(desiredPosition.x() < levelLeft)
        {
            desiredPosition.setX(levelLeft);
            // Stoppe horizontale Geschwindigkeit, wenn gegen linke Wand
            if(m_actor->velocity().x() < 0)
            {
                m_actor->velocity().setX(0);
            }
        }
        
        // Prüfe rechte Grenze (berücksichtige Sprite-Breite)
        if(desiredPosition.x() + m_actor->w() > levelRight)
        {
            desiredPosition.setX(levelRight - m_actor->w());
            // Stoppe horizontale Geschwindigkeit, wenn gegen rechte Wand
            if(m_actor->velocity().x() > 0)
            {
                m_actor->velocity().setX(0);
            }
        }
    }

    m_actor->setWorldPosition(desiredPosition);
}

} // namespace jumper
