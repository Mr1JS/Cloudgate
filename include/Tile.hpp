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
