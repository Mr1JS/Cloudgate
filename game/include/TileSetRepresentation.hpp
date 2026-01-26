/*
 *  TileSetRepresentation.hpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */

#ifndef UEBUNG08_SRC_TILESETREPRESENTATION_HPP_
#define UEBUNG08_SRC_TILESETREPRESENTATION_HPP_

/// Proxy class to implement [][]-access
class TileIndexProxy
{
public:
    virtual int operator[](int x) = 0;
};


/**
 * @brief The TileSetRepresentation class is an interface
 *        for all supported representations of tile sets that
 *        can be rendered by the \ref TileSet class.
 */
class TileSetRepresentation
{
public:

    /**
     * @brief TileSetRepresentation constructor
     * @param x     x position
     * @param y     y position
     * @param w     tile set width
     * @param h     tile set height
     */
    TileSetRepresentation(int x, int y, int w, int h, int tileW, int tileH) :
        m_x(x), m_y(y), m_w(w), m_h(h), m_tileWidth(tileW), m_tileHeight(tileH) {}

    /// Inserts a tile index at position (x, y)
    virtual void insert(int x, int y, int value) = 0;

    /// Returns the tile index at position (x, y)
    virtual int  get(int x, int y) const = 0;

    /// Destructor
    virtual ~TileSetRepresentation() {}

    /// Returns the number of managed columns
    int width() const { return m_w; }

    /// Retruns the number of managed rows
    int height() const { return m_h; }

    /// Returns the tile height
    int tileHeight() const { return m_tileHeight; }

    /// Retruns the tile width
    int tileWidth() const { return m_tileWidth; }

protected:
    /// x position
    int 			m_x;

    /// y position
    int 			m_y;

    /// Tile set width
    int				m_w;

    /// Tile set height
    int				m_h;

    /// Tile with
    int             m_tileWidth;

    /// Tile height
    int             m_tileHeight;



};


#endif /* UEBUNG08_SRC_TILESETREPRESENTATION_HPP_ */
