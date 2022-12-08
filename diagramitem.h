#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

#include <QGraphicsPixmapItem>
#include <QList>

QT_BEGIN_NAMESPACE
class QPixmap;
class QGraphicsSceneContextMenuEvent;
class QMenu;
class QPolygonF;
QT_END_NAMESPACE

class Arrow;

class DiagramItem : public QGraphicsPolygonItem
{
public:
    DiagramItem(int i_value, QGraphicsItem *parent = nullptr);

    void removeArrow(Arrow *arrow);
    void removeArrows();
    QPolygonF polygon() const { return myPolygon; }
    void addArrow(Arrow *arrow);
    int getValue() { return value; };
    void setValue(int i_value);


protected:
//    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    QPolygonF myPolygon;
//    QMenu *myContextMenu;
    QList<Arrow *> arrows;
    QGraphicsTextItem *text;
    int value;
};

#endif // DIAGRAMITEM_H
