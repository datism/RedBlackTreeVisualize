#ifndef DIAGRAMITEM_H
#define DIAGRAMITEM_H

#include <QGraphicsPolygonItem>

#define LEFT  0
#define RIGHT 1
#define PARENT 2
#define LEFT_CHILD  child[LEFT]
#define RIGHT_CHILD child[RIGHT]
#define CHILD_DIR(n) ( n == (n->parent)->RIGHT_CHILD ? RIGHT : LEFT )

#define DIAGONAL 20

class Arrow;

class RBnode : public QGraphicsPolygonItem
{
public:
    enum Color {BLACK, RED};
    RBnode* parent;
    RBnode* child[2];
    int key;
    Color color;

    Arrow* arrows[3];

    RBnode(int i_key, QGraphicsItem *parent = nullptr);

//    void removeArrow(Arrow *arrow);
//    void removeArrows();
    QPolygonF polygon() const { return myPolygon; }
//    void addArrow(Arrow *arrow);
    void select(){isSelected=true;};
    void unSelect(){isSelected=false;};

    friend void swap(RBnode *a, RBnode *b);
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

private:
    QPolygonF myPolygon;
    QGraphicsTextItem *text;
    QPen *pen;
    QBrush *brush;
    bool isSelected;
};

#endif // DIAGRAMITEM_H
