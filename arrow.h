#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsLineItem>

class RBnode;

class Arrow : public QGraphicsLineItem
{
public:
    enum { Type = UserType + 4 };

    Arrow(RBnode *startItem, RBnode *endItem,
          QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void setColor(const QColor &color) { myColor = color; }
    RBnode *startItem() const { return myStartItem; }
    RBnode *endItem() const { return myEndItem; }

    void updatePosition();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    RBnode *myStartItem;
    RBnode *myEndItem;
    QPolygonF arrowHead;
    QColor myColor = Qt::black;
};

#endif // ARROW_H
