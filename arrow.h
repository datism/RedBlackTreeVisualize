#ifndef ARROW_H
#define ARROW_H

#include <QGraphicsLineItem>

class RBnode;

class Arrow : public QGraphicsLineItem
{
public:
    Arrow(RBnode *startItem, RBnode *endItem,
          QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    RBnode *startItem() const { return myStartItem; }
    RBnode *endItem() const { return myEndItem; }

    void updatePosition();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    RBnode *myStartItem;
    RBnode *myEndItem;
};

#endif // ARROW_H
