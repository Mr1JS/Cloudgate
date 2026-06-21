/**
 * @file Tile.hpp
 * @brief Defines the Tile struct for representing a single tile in the level editor
 */

#ifndef TILE_HPP
#define TILE_HPP

#include <QPixmap>
#include <QRect>

/// Struct for tile management
struct Tile
{
    int index = -1; /// Index of tile
    QPixmap pixmap; /// Image data
    QRect sourceRect; /// Coordinates from the tileset
}; 

#endif // TILE_HPP
