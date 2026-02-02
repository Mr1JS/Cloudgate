/*
 *  TileSet.hpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef UEBUNG08_SRC_TILESET_HPP_
#define UEBUNG08_SRC_TILESET_HPP_

#include "LevelParser.hpp"
#include "StaticRenderable.hpp"
#include "TileSetRepresentation.hpp"

namespace jumper
{

/**
 * @brief   Represents an array of tiles and provides
 *          rendering fuctionality
 */
class TileSet : public StaticRenderable
{
  public:
    /**
     * @brief creates TileSet
     *
     * @param mw Pointer to main window
     * @param io hdf5 io that has the resourced file open
     * @param tilesDataset name of hdf5 dataset that holds the tiles array
     * @param tilesetDataset name of the hdf5 dataset that holds the tileset
     * @param tileWidth width of one tile
     * @param tileHeight height of one tile
     * @param tilesPerRow number of tiles per row
     * @param numRows amount of rows
     * @param tileOffset pixel offset between tiles in tileset
     */
    TileSet(MainWindow* mw,
            LevelParser::LevelHdf5IO& io,
            std::string tilesDataset,
            std::string tilesetDataset,
            int tileWidth,
            int tileHeight,
            int tilesPerRow,
            int numRows,
            int tileOffset);

    /**
     * @brief TileArray
     * @param mw Pointer to a main window
     * @param levelFile Name of a level file to parse
     */
    TileSet(MainWindow* mw, std::string levelFile);

    /// Renders the tile array
    virtual void render() override;

    /// Returns the width of the rendering area
    int w();

    /// Retruns the height of the rendering area
    int h();

    /// Destructor
    virtual ~TileSet();

    TileSetRepresentation* tiles()
    {
        return m_tiles;
    }

    SDL_Texture* texture() const { return m_texture; }
    int tileWidth() const { return m_tileWidth; }
    int tileHeight() const { return m_tileHeight; }
    int tilesPerRow() const { return m_tilesPerRow; }
    int tileOffset() const { return m_tileOffset; }

  private:
    /// With of the tiles
    int m_tileWidth;

    /// Height of the tiles
    int m_tileHeight;

    /// Width of the array (i.e. number of tile colums)
    int m_arrayWidth;

    /// Height of the array (i.e. number of tile rows)
    int m_arrayHeight;

    /// Number of tiles per row in the tile sheet
    int m_tilesPerRow;

    /// Number of rows in the tile sheet
    int m_numRows;

    /// Pixel offset between the tiles in the tile sheet
    int m_tileOffset;

    /// Internal represention of the tiles
    TileSetRepresentation* m_tiles;
};

} /* namespace jumper */

#endif /* UEBUNG08_SRC_TILESET_HPP_ */
