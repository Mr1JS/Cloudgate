/**
 * @file Tile.hpp
 * @brief Defines the Tile struct for representing a single tile in the level editor
 */

#ifndef TILE_HPP
#define TILE_HPP

#include <QPixmap>
#include <QRect>

struct Tile
{
    int index = -1;
    QPixmap pixmap;
    QRect sourceRect;
};

#endif // TILE_HPP
