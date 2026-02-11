/*
 *  LevelParser.hpp
 *  Created on: Jan 07, 2018
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2018 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef LEVELPARSER_H
#define LEVELPARSER_H

#include <string>

#include "ActorForces.hpp"
#include "LevelForces.hpp"
#include "game/io/BaseHdf5IO.hpp"
#include "game/io/TextureIO.hpp"
#include "game/io/TileSetIO.hpp"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

namespace jumper
{

class Level;
class MainWindow;

class LevelParser
{
public:
    using LevelHdf5IO = BaseHdf5IO<hdf5features::TextureIO, hdf5features::TileSetIO>;

    /**
     * Parse a level from the given file and set up the given level accordingly
     */
    LevelParser(std::string file, Level* level, MainWindow* mw);

private:

    /// A pointer to the level that is set up
    Level*          m_level;

    /// A pointer to the responsible main window
    MainWindow*     m_mainWindow;
};

}
#endif // LEVELPARSER_H
