/*
 *  TileArray.hpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef TILEARRAY_H
#define TILEARRAY_H

#include "TileSetRepresentation.hpp"

namespace jumper
{

/**
 * @brief   The TileArray class represents a set of tiles
 *          in a 2D array.
 */
class TileArray : public TileSetRepresentation
{
public:

    /**
     * @brief TileArray constructor
     * @param x     x position of the tile array
     * @param y     y position of the tile array
     * @param w     array width
     * @param h     array height
     */
    TileArray(int x, int y, int w, int h, int tileW, int tileH);

    /**
     * @brief Gets the tile index at position (x, y) in the array
     */
    virtual int get(int x, int y) const override;

    /**
     * @brief Inserts a tile index at position (x, y) into the array
     */
    virtual void insert(int x, int y, int value) override;

    /// Destructor
    virtual ~TileArray();

    /// Index acces operator
    int* operator[](int i);

private:

    /// 2D int array to store the tile indices
    int**	m_tiles;
};

} // namespace jumper

#endif // TILEARRAY_H
