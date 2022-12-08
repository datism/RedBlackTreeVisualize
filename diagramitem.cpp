// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "diagramitem.h"
#include "arrow.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>

#define SIZE 20

DiagramItem::DiagramItem(int i_value, QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent)
{
    myPolygon << QPointF(-SIZE, -SIZE) << QPointF(SIZE, -SIZE)
              << QPointF(SIZE, SIZE) << QPointF(-SIZE, SIZE)
              << QPointF(-SIZE, -SIZE);

    setPolygon(myPolygon);

    text = new QGraphicsTextItem(this);
    text->setTextWidth(this->boundingRect().width());
    QRectF rect = text->boundingRect();
    rect.moveCenter(this->boundingRect().center());
    text->setPos(rect.topLeft());

    setValue(i_value);

//    setFlag(QGraphicsItem::ItemIsMovable, true);
//    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}


void DiagramItem::removeArrow(Arrow *arrow)
{
    arrows.removeAll(arrow);
}

void DiagramItem::removeArrows()
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

void DiagramItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}

void DiagramItem::setValue(int i_value)
{
    value = i_value;
    text->setHtml(QStringLiteral("<center>%1</center>").arg(value));
}

//QPixmap DiagramItem::image() const
//{
//    QPixmap pixmap(250, 250);
//    pixmap.fill(Qt::transparent);
//    QPainter painter(&pixmap);
//    painter.setPen(QPen(Qt::black, 8));
//    painter.translate(125, 125);
//    painter.drawPolyline(myPolygon);

//    return pixmap;
//}

//void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
//{
//    scene()->clearSelection();
//    setSelected(true);
//    myContextMenu->exec(event->screenPos());
//}

QVariant DiagramItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        for (Arrow *arrow : qAsConst(arrows))
            arrow->updatePosition();
    }

    return value;
}

