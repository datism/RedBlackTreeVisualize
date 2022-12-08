#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include "rbtree.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();
private slots:
    void addNode();
    void deleteNode();
private:
    Ui::Dialog *ui;
    RBtree tree;
    QGraphicsScene *scene;
};
#endif // DIALOG_H
