#include "TileSet.hpp"

TileSet::TileSet(
    int index,
    int sourceX,
    int sourceY,
    int width,
    int height,
    const QString& image,
    QObject* parent
    )
    : QObject(parent),
    m_index(index),
    m_sourceX(sourceX),
    m_sourceY(sourceY),
    m_width(width),
    m_height(height),
    m_image(image)
{
}

int TileSet::index() const { return m_index; }
int TileSet::sourceX() const { return m_sourceX; }
int TileSet::sourceY() const { return m_sourceY; }
int TileSet::width() const   { return m_width; }
int TileSet::height() const  { return m_height; }
QString TileSet::image() const { return m_image; }
