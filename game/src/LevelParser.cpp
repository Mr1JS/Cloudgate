/*
 *  LevelParser.cpp
 *  Created on: Jan 07, 2018
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2018 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "include/LevelParser.hpp"

#include "include/Actor.hpp"
#include "include/Level.hpp"
#include "include/MainWindow.hpp"
#include "include/Util.hpp"

namespace jumper
{

LevelParser::LevelParser(std::string filename, Level* level, MainWindow* mw)
    : m_level(level), m_mainWindow(mw)
{
    // Get path from given file name
    std::size_t found = filename.find_last_of("/\\");
    std::string path  = filename.substr(0, found);

    // Read property tree from xml file
    using boost::property_tree::ptree;
    ptree pt;
    read_xml(filename, pt);

    LevelHdf5IO io;
    io.open(path + "/" + pt.get<std::string>("level.<xmlattr>.resources"));

    // Parse all children of the level tag
    BOOST_FOREACH (const ptree::value_type& v, pt.get_child("level"))
    {
        if (v.first == "collision_tiles")
        {
            // Parse attributes
            std::string tilesDataset   = v.second.get("<xmlattr>.tiles", "");
            std::string tilesetDataset = v.second.get("<xmlattr>.texture", "");
            int layer                  = v.second.get<int>("layer", 0);
            int tileWidth              = v.second.get("tileWidth", 0);
            int tileHeight             = v.second.get("tileHeight", 0);
            int tilesPerRow            = v.second.get("tilesPerRow", 0);
            int numRows                = v.second.get("numRows", 0);
            int tileOffset             = v.second.get("tileOffset", 0);

            // Create and setup tile set
            TileSet* ts = new TileSet(m_mainWindow,
                                      io,
                                      tilesDataset,
                                      tilesetDataset,
                                      tileWidth,
                                      tileHeight,
                                      tilesPerRow,
                                      numRows,
                                      tileOffset);

            // Add tile set to level
            m_level->addLevelTiles(ts, layer);
        }

        if (v.first == "background_tiles")
        {
            // Parse attributes
            std::string tilesDataset   = v.second.get("<xmlattr>.tiles", "");
            std::string tilesetDataset = v.second.get("<xmlattr>.texture", "");
            int layer                  = v.second.get<int>("layer", 0);
            int tileWidth              = v.second.get("tileWidth", 0);
            int tileHeight             = v.second.get("tileHeight", 0);
            int tilesPerRow            = v.second.get("tilesPerRow", 0);
            int numRows                = v.second.get("numRows", 0);
            int tileOffset             = v.second.get("tileOffset", 0);

            // Create and setup tile set
            TileSet* ts = new TileSet(m_mainWindow,
                                      io,
                                      tilesDataset,
                                      tilesetDataset,
                                      tileWidth,
                                      tileHeight,
                                      tilesPerRow,
                                      numRows,
                                      tileOffset);

            // Add tile set to level
            m_level->addRenderable(ts, layer);
        }

        if (v.first == "actor")
        {
            // Parse attributes
            std::string textureDataset = v.second.get("<xmlattr>.texture", "");
            int numFrames              = v.second.get<int>("num_frames", 0);
            int frameWidth             = v.second.get<int>("frame_width", 0);
            int frameHeight            = v.second.get<int>("frame_height", 0);
            int fps                    = v.second.get<int>("fps", 14);
            int layer                  = v.second.get<int>("layer", 0);

            int pos_x           = v.second.get<int>("position_x", 0);
            int pos_y           = v.second.get<int>("position_y", 0);
            float jumpForceX    = v.second.get<float>("jump_force_x", 0.0);
            float jumpForceY    = v.second.get<float>("jump_force_y", 0.0);
            float moveForceX    = v.second.get<float>("move_force_x", 0.0);
            float moveForceY    = v.second.get<float>("move_force_y", 0.0);
            float maxVelRun     = v.second.get<float>("max_run_velocity", 0.0);
            float maxVelFall    = v.second.get<float>("max_fall_velocity", 0.0);
            float maxJumpHeight = v.second.get<float>("max_jump_height", 0.0);

            // Create new actor
            SDL_Surface* surface = io.TextureIO::load("textures", textureDataset);
            if (surface == nullptr)
            {
                throw std::invalid_argument("The image " + textureDataset
                                            + " could not be loaded from hdf5!");
            }
            SDL_Texture* tex = SDL_CreateTextureFromSurface(mw->renderer(), surface);
            SDL_FreeSurface(surface);
            if (tex == nullptr)
            {
                throw std::invalid_argument("Could not create texture from image " + textureDataset
                                            + "!");
            }

            // Create new actor
            Actor* actor = new Actor(m_mainWindow, tex, frameWidth, frameHeight, numFrames);

            // Create forces
            ActorForces f(Vector2f(moveForceX, moveForceY),
                          Vector2f(jumpForceX, jumpForceY),
                          maxVelRun,
                          maxVelRun,
                          maxJumpHeight,
                          maxVelFall);

            // Setup actor
            actor->setFPS(fps);
            actor->setPhysics(f);
            actor->setWorldPosition(Vector2f(pos_x, pos_y));

            // Add tile set to level
            m_level->addActor(actor, layer);
        }

        if (v.first == "level_forces")
        {
            float gravity_x = v.second.get<float>("gravity_x", 0.0);
            float gravity_y = v.second.get<float>("gravity_y", 0.0);
            float damping_x = v.second.get<float>("damping_x", 1.0);
            float damping_y = v.second.get<float>("damping_y", 1.0);

            LevelForces f(Vector2f(gravity_x, gravity_y), Vector2f(damping_x, damping_y));

            m_level->setForces(f);
        }
    }
}

} // namespace jumper

