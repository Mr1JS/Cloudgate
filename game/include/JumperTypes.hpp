/* Copyright (C) 2016 Uni Osnabrück
 * This file is part of the LAS VEGAS Reconstruction Toolkit,
 *
 * LAS VEGAS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * LAS VEGAS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 */


/*
 * JumperTypes.hpp
 *
 *  Created on: Jan 6, 2017
 *      Author: Thomas Wiemann (twiemann@uos.de)
 */

#include <list>
#include <tuple>

#ifndef _JUMPERTYPES_HPP_
#define _JUMPERTYPES_HPP_

namespace jumper
{

/**
 * @brief x y tileId
 */
typedef std::tuple<int, int, int> Tile;
typedef std::list<Tile>			  TileList;

}

#endif /* SOFTWARE_MUSTERLOESUNGEN_UEBUNG10_SRC_JUMPERTYPES_HPP_ */
