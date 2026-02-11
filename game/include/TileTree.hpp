/*
 *  TileTree.hpp
 *  Created on: Dec 08, 2017
 *      Author: Thomas Wiemann
 *
 *  Copyright (c) 2017 Thomas Wiemann.
 *  Restricted usage. Licensed for participants of the course "The C++ Programming Language" only.
 *  No unauthorized distribution.
 */


#ifndef TILETREE_HPP_
#define TILETREE_HPP_

#include "TileSetRepresentation.hpp"
#include "Camera.hpp"
#include "JumperTypes.hpp"

#include <SDL.h>

namespace jumper
{

/// Flag for split axis
enum SplitAxis
{
    HORIZONTAL,
    VERTICAL
};

/**
 * @brief The TileTree class represents a set of tiles in a kd tree
 */
template<typename T>
class TileTree : public TileSetRepresentation
{
^public:

    /**
     * @brief	Internal helper class to allow [][] access to the tiles
     */
    class IndexHelper: public TileIndexProxy
    {
    public:
        /// Constructor with pointer to parent tree and Y position of access
        IndexHelper(TileTree* parent, int y) : m_parent(parent), m_y(y) {}

        /// Parent tree. Used to access the tiles via \ref get
        TileTree* m_parent;

        /// Y position of access
        int m_y;

        /// Operator to access element in parent tree
        T operator[](int x);

    };

    /***
     * @brief 	Creates a new tile tree at position \ref x, \ref y with width \ref w
     * 			and height \ref h. \ref axis defines the split axis on which the
     * 			first split is performed. The position and bounding box information
     * 			is given in tile coordinates.
     */
    TileTree(int x, int y, int w, int h, int tileW, int tileH, SplitAxis axis = VERTICAL);

    /**
     * @brief	Inserts a \ref element at the given *grid* position, hence \ref i
     * 			and \ref j reference indices in the tile grid
     */
    void insert(int x, int y, T element) override;

    /**
     * @brief	Returns the tile at the given position
     */
    T get(int x, int y) const override;

    /// Destructor
    virtual ~TileTree();

    /// Index access
    virtual IndexHelper operator[](int y) { return IndexHelper(this, y);}

    void clip(const Camera& cam, TileList& list);

protected:

    /// Returns the split position on the grid (x or y) depending
    /// on the current split axis orientation
    int getSplitPosition(const SplitAxis& axis) const;

    /// Returns a SDL_Rect representation of the tree volume
    SDL_Rect getRect() const;

    /// Split axis orientation
    SplitAxis 		m_axis;

    /// Upper / left sub tree (depending on split orientation)
    TileTree* 		m_sub1;

    /// Lower / right sub tree (depending on split orientation)
    TileTree* 		m_sub2;

    /// Array of tiles stored in the leafs
    T*	        m_tiles;

    /// Number of tiles on the tile leaf array
    int				m_numTiles;
};

}

#include "TileTree.tcc"

#endif /* TILETREE_HPP_ */
