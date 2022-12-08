#include "dialog.h"
#include "./ui_dialog.h"
#include "diagramitem.h"

#include <QScreen>
#include <QPushButton>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    connect(ui->addButton, &QPushButton::clicked, this, &Dialog::addNode);

//    scene->addText("Hello, world");
//    scene->setSceneRect(0, 0, 5000, 5000);
}

void Dialog::addNode()
{
    DiagramItem *item = new DiagramItem(50054);
    scene->addItem(item);
    item->setPos(5000, 5000);
}

void Dialog::deleteNode()
{

}

Dialog::~Dialog()
{
    delete ui;
}

