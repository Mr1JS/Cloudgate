/*
 *  TileSet.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "game/include/TileSet.hpp"

#include "game/include/MainWindow.hpp"
#include "game/include/TileArray.hpp"
#include "game/include/TileTree.hpp"
#include "game/include/Util.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace jumper
{

TileSet::TileSet(MainWindow* mw,
                 LevelParser::LevelHdf5IO& io,
                 std::string tilesDataset,
                 std::string tilesetDataset,
                 int tileWidth,
                 int tileHeight,
                 int tilesPerRow,
                 int numRows,
                 int tileOffset)
    : StaticRenderable(mw), m_tileWidth(tileWidth), m_tileHeight(tileHeight),
      m_tilesPerRow(tilesPerRow), m_numRows(numRows), m_tileOffset(tileOffset)
{
    SDL_Surface* surface = io.TextureIO::load("textures", tilesetDataset);
    if (surface == nullptr)
    {
        throw std::invalid_argument("The image " + tilesetDataset
                                    + " could not be loaded from hdf5!");
    }
    m_texture = SDL_CreateTextureFromSurface(mw->renderer(), surface);
    SDL_FreeSurface(surface);
    if (m_texture == nullptr)
    {
        throw std::invalid_argument("Could not create texture from image " + tilesetDataset + "!");
    }

    m_tiles
        = io.TileSetIO::template load<TileArray>("tiles", tilesDataset, m_tileWidth, m_tileHeight);
        //= io.TileSetIO::template load<TileTree<int>>("tiles", tilesDataset, m_tileWidth, m_tileHeight);
    m_arrayWidth  = m_tiles->width();
    m_arrayHeight = m_tiles->height();
}

TileSet::TileSet(MainWindow* mw, std::string filename) : StaticRenderable(mw)
{
    // Read meta data from level file
    std::ifstream in(filename.c_str());
    std::string texFileName, actorFileName;

    // Get path from given .lvl file
    size_t position  = filename.find_last_of("/\\");
    std::string path = filename.substr(0, position + 1);

    int ir, ig, ib;
    unsigned char keyR, keyG, keyB;

    if (in.good())
    {
        in >> texFileName;
        in >> actorFileName;
        in >> m_tileWidth >> m_tileHeight >> m_tilesPerRow >> m_numRows;
        in >> m_tileOffset >> ir >> ig >> ib;
        in >> m_arrayWidth >> m_arrayHeight;
    }
    else
    {
        std::cout << "Unable to open file " << filename << std::endl;
    }

    // Cast keying colors manually!
    keyR = (unsigned char)ir;
    keyG = (unsigned char)ib;
    keyB = (unsigned char)ig;

    // Load textures
    m_texture = LoadTexture(m_mainWindow->renderer(), path + texFileName, keyR, keyG, keyB);

    /*m_tiles = new TileArray(0,
                            0,
                            m_arrayWidth,
                            m_arrayHeight,
                            m_tileWidth,
                            m_tileHeight);*/ // Corresponding to old representation
    m_tiles = new TileTree<int>(0, 0, m_arrayWidth, m_arrayHeight, m_tileWidth, m_tileHeight);  //
    // Uncomment this to use your tile tree

    // Read tile indices
    for (int i = 0; i < m_arrayHeight; i++)
    {
        for (int j = 0; j < m_arrayWidth; j++)
        {
            int tileID;
            in >> tileID;
            m_tiles->insert(j, i, tileID);
        }
    }

    // Close file
    in.close();
}

TileSet::~TileSet()
{
    if (m_tiles)
    {
        delete m_tiles;
    }
    SDL_DestroyTexture(m_texture);
}

int TileSet::w()
{
    return m_arrayWidth * m_tileWidth;
}

int TileSet::h()
{
    return m_tileHeight * m_arrayHeight;
}

void TileSet::render()
{
    if (readyToRender())
    {
        int i;
        int j;
        int tile_index;
        int col, row;
        SDL_Rect target;
        SDL_Rect source;

        /* Set target / source width and height to tile size,
         * they never change
         */
        target.w = m_tileWidth;
        target.h = m_tileHeight;

        source.w = m_tileWidth;
        source.h = m_tileHeight;

        for (i = 0; i < m_arrayHeight; i++)
        {
            for (j = 0; j < m_arrayWidth; j++)
            {
                // tile_index = m_tiles[i][j] - 1;
                tile_index = m_tiles->get(j, i) - 1;
                if (tile_index >= 0)
                {
                    /* Compute the position of the target on the screen */
                    // Y-Offset von 600, damit die Karte weiter unten startet
                    target.x = j * m_tileWidth - m_offset.x() + m_windowOffset.x();
                    target.y = i * m_tileHeight - m_offset.y() + m_windowOffset.y() + 600;

                    /* Compute the position of the source pixel data
                     * within the texture (no offset for first tiles)
                     */
                    row = tile_index / m_tilesPerRow;
                    col = tile_index % m_tilesPerRow;

                    source.x = col * m_tileWidth;
                    if (col > 0)
                    {
                        source.x += col * m_tileOffset;
                    }

                    source.y = row * m_tileHeight;
                    if (row > 0)
                    {
                        source.y += row * m_tileOffset;
                    }

                    /* Copy pixel date to correct position */
                    SDL_RenderCopy(m_mainWindow->renderer(), m_texture, &source, &target);
                }
            }
        }
    }
}

} // namespace jumper

