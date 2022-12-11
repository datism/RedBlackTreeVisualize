// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "diagramitem.h"
#include "arrow.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>

RBnode::RBnode(int i_key, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent), key(i_key), color(RED), parent(NULL)
{
    RIGHT_CHILD = NULL;
    LEFT_CHILD = NULL;

//    myPolygon << QPointF(-SIDE, -SIDE) << QPointF(SIDE, -SIDE)
//              << QPointF(SIDE, SIDE) << QPointF(-SIDE, SIDE)
//              << QPointF(-SIDE, -SIDE);

    myPolygon << QPointF(0, 0) << QPointF(SIDE, 0)
              << QPointF(SIDE, SIDE) << QPointF(0, SIDE);

    setPolygon(myPolygon);


    text = new QGraphicsTextItem(this);
    text->setTextWidth(this->boundingRect().width());
    QRectF rect = text->boundingRect();
    rect.moveCenter(this->boundingRect().center());
    text->setPos(rect.topLeft());
    text->setHtml(QStringLiteral("<center>%1</center>").arg(key));

    brush = new QBrush(Qt::white, Qt::SolidPattern);
    pen = new QPen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    setPen(*pen);

//    setFlag(QGraphicsItem::ItemIsMovable, true);
//    setFlag(QGraphicsItem::ItemIsSelectable, true);
//    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

void RBnode::select()
{
    brush->setColor(Qt::green);
    setBrush(*brush);
}

void RBnode::unSelect()
{
    brush->setColor(Qt::white);
    setBrush(*brush);
}


void RBnode::removeArrow(Arrow *arrow)
{
    arrows.removeAll(arrow);
}

void RBnode::removeArrows()
{
    // need a copy here since removeArrow() will
    // modify the arrows container
    const auto arrowsCopy = arrows;
    for (Arrow *arrow : arrowsCopy) {
        arrow->startItem()->removeArrow(arrow);
        arrow->endItem()->removeArrow(arrow);
        scene()->removeItem(arrow);
        delete arrow;
    }
}

void RBnode::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}


QVariant RBnode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        for (Arrow *arrow : qAsConst(arrows))
            arrow->updatePosition();
    }
//    else if (change == QGraphicsItem::ItemSelectedChange) {
//        if (value.toBool() == true)
//            pen->setColor(Qt::green);
//        else if (color == RED)
//            pen->setColor(Qt::red);
//        else if (color == BLACK)
//            pen->setColor(Qt::black);

//        setPen(*pen);
//    }

    return value;
}

//void swap(RBnode *a, RBnode *b)
//{
//    RBnode::Color tempC;
//    int temK;

//    tempC = a->color;
//    temK = a->key;

//    a->color = b->color;
//    a->key = b->key;

//    b->color = tempC;
//    b->key = temK;
//}
