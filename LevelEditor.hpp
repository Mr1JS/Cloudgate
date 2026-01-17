#include <QQuickPaintedItem>
#include <QPixmap>
#include <QList>
#include <QRect>

// save tiles with index, pixmap and soureRect
struct Tile {
    int index;
    QPixmap pixmap;
    QRect sourceRect;
};

class LevelEditor : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit LevelEditor(QQuickItem* parent = nullptr);

    Q_INVOKABLE void loadTileset(const QString& path, int tileW = 32, int tileH = 32);

    void paint(QPainter* painter) override;

signals:
    void tileClicked(int index);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    QList<Tile> m_tiles;
    int m_tileWidth = 32; // aalways the same values because only one tileset
    int m_tileHeight = 32;
    int m_tileOffset = 4;
    int m_selectedIndex = -1; // nothing selected yet
};
