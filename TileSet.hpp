#pragma once

#include <QObject>

class TileSet : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int index READ index CONSTANT)
    Q_PROPERTY(int sourceX READ sourceX CONSTANT)
    Q_PROPERTY(int sourceY READ sourceY CONSTANT)
    Q_PROPERTY(int width READ width CONSTANT)
    Q_PROPERTY(int height READ height CONSTANT)
    Q_PROPERTY(QString image READ image CONSTANT)

public:
    TileSet(
        int index,
        int sourceX,
        int sourceY,
        int width,
        int height,
        const QString& image,
        QObject* parent = nullptr
        );

    int index() const;
    int sourceX() const;
    int sourceY() const;
    int width() const;
    int height() const;
    QString image() const;

private:
    int m_index;
    int m_sourceX;
    int m_sourceY;
    int m_width;
    int m_height;
    QString m_image;
};
