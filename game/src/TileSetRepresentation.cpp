/**
 * @file TileSetRepresentation.cpp
 * @brief Implementation of the TileSetRepresentation class for mapping between
 *        tile coordinates, pixel positions and physical properties of tiles
 */

#include "game/include/TileSetRepresentation.hpp"

namespace jumper
{

int TileSetRepresentation::width() const
{
    return m_w;
}

int TileSetRepresentation::height() const
{
    return m_h;
}

int TileSetRepresentation::tileHeight() const
{
    return m_tileHeight;
}

int TileSetRepresentation::tileWidth() const
{
    return m_tileWidth;
}

} // namespace jumper
