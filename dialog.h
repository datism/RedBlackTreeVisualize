#ifndef DIALOG_H
#define DIALOG_H

#include "diagramitem.h"

#include <QTextEdit>
#include <QDialog>
#include <QGraphicsScene>

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
    void nextStep();
private:
    Ui::Dialog *ui;
    QGraphicsScene *scene;
    RBnode *root;
    QVector<int> readInput(QTextEdit *text);
    void add(int val);
    void remove(int val);
    void fixRedRed(RBnode *n, RBnode *p, int dir);
    RBnode* RotateDirRoot(RBnode *p, int dir);
    void fixCollision(RBnode *n);
    void movePos(RBnode *n, const QPointF &dist);
    void addArrow(RBnode *p, RBnode *n, int d);
    void removeArrow(RBnode *n, int i);
    void updateAndWait();

    bool next;
    RBnode *findReplace(RBnode *n);
    RBnode *findNode(int val);
    void fixDoubleBlack(RBnode *n);
};
#endif // DIALOG_H
