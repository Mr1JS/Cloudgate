/**
 * @file TileArray.cpp
 * @brief Implementation of the TileArray class for efficient storage and management
 *        of 2D tile data with fast access to individual tiles in the level grid
 */

/*
 *  TileArray.cpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#include "game/include/TileArray.hpp"
#include <iostream>
using namespace std;

namespace jumper
{

TileArray::TileArray(int x, int y, int w, int h, int tileW, int tileH)
    : TileSetRepresentation(x, y, w, h, tileW, tileH)
{
    // Alloc tile array and initialize empty
    m_tiles = new int*[m_w];
    for (int i = 0; i < m_w; i++)
    {
        m_tiles[i] = new int[m_h];
        for (int j = 0; j < m_h; j++)
        {
            m_tiles[i][j] = 0;
        }
    }
}

int* TileArray::operator[](int i)
{
    return m_tiles[i];
}


int TileArray::get(int x, int y) const
{
    // Prüfe Grenzen: x und y müssen >= 0 und < width/height sein
    if (x >= 0 && x < m_w && y >= 0 && y < m_h)
    {
        return m_tiles[x][y];
    }
    else
    {
        return -1;
    }
}

void TileArray::insert(int x, int y, int value)
{
    if (x < m_w && y < m_h)
    {
        m_tiles[x][y] = value;
    }
}

TileArray::~TileArray()
{
    for (int i = 0; i < m_w; i++)
    {
        delete[] m_tiles[i];
    }
    delete[] m_tiles;
}

} // namespace jumper
