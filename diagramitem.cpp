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
    arrows[LEFT] = NULL;
    arrows[RIGHT] = NULL;
    arrows[PARENT] = NULL;
    isSelected = false;

    myPolygon << QPointF(-DIAGONAL, -DIAGONAL) << QPointF(DIAGONAL, -DIAGONAL)
              << QPointF(DIAGONAL, DIAGONAL) << QPointF(-DIAGONAL, DIAGONAL)
              << QPointF(-DIAGONAL, -DIAGONAL);

    setPolygon(myPolygon);

    text = new QGraphicsTextItem(this);
    text->setDefaultTextColor(Qt::white);
    text->setTextWidth(this->boundingRect().width());
    QRectF rect = text->boundingRect();
    rect.moveCenter(this->boundingRect().center());
    text->setPos(rect.topLeft());
    text->setHtml(QStringLiteral("<center>%1</center>").arg(key));

    brush = new QBrush(Qt::red, Qt::SolidPattern);
    setBrush(*brush);
    pen = new QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    setPen(*pen);

    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

void RBnode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Qt::GlobalColor c = (color == RED) ? Qt::red : Qt::black;

    if (isSelected)
        pen->setColor(Qt::green);
    else
        pen->setColor(c);
    setPen(*pen);

    brush->setColor(c);
    setBrush(*brush);

    text->setHtml(QStringLiteral("<center>%1</center>").arg(key));

    QGraphicsPolygonItem::paint(painter, option, widget);
}

QVariant RBnode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        for (Arrow *arrow : arrows)
            if (arrow != NULL)
                arrow->updatePosition();
    }

    return value;
}

void swap(RBnode *a, RBnode *b)
{
    RBnode::Color tempC;
    int temK;

    tempC = a->color;
    temK = a->key;

    a->color = b->color;
    a->key = b->key;

    b->color = tempC;
    b->key = temK;
}
