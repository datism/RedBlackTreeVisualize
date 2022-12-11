#include "dialog.h"
#include "./ui_dialog.h"
#include "arrow.h"

#include <QScreen>
#include <QPushButton>

#define ROTATE_DIR(n,dir) RotateDirRoot(n,dir)
#define ROTATE_LEFT(n)    RotateDirRoot(n,LEFT)
#define ROTATE_RIGHT(n)   RotateDirRoot(n,RIGHT)

#define WGAP DIAGONAL * 2
#define HGAP DIAGONAL * 4
#define DIST_DIR(dist,dir) (2*dir-1)*dist

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    connect(ui->addButton, &QPushButton::clicked, this, &Dialog::addNode);
    connect(ui->deleteButton, &QPushButton::clicked, this, &Dialog::deleteNode);

    root = NULL;

    scene->setSceneRect(0, 0, 5000, 5000);
    ui->graphicsView->centerOn(scene->width() / 2, 0);
}

QVector<int> Dialog::readInput(QTextEdit *textEdit)
{
   QVector<int> arr;
   QString text = textEdit->toPlainText();
   text = text.removeIf([](auto c){return c.isSpace();});

   bool ok;
   int val;
   for (auto it : QStringTokenizer{text, u','})
   {
      val = it.toInt(&ok);
      if (ok)
        arr.append(val);
   }

   return arr;
}

void Dialog::addNode()
{

    QVector<int> arr = readInput(ui->addTextEdit);

    for (auto v: arr)
    {
        add(v);
    }

//    RBnode *cur = new RBnode(100);
//    scene->addItem(cur);
//    cur->setPos(scene->width() / 2, HGAP);

//    RBnode *n = new RBnode(1000);
//    scene->addItem(n);
//    n->setPos(cur->pos() + QPointF(WGAP, HGAP));


//    Arrow *arrow = new Arrow(cur, n);
//    cur->addArrow(arrow);
//    n->addArrow(arrow);
//    arrow->setZValue(-1000);
//    scene->addItem(arrow);
//    arrow->updatePosition();


//    cur->select();

//    item->select();
//    item->unSelect();
}

void Dialog::deleteNode()
{

}

void Dialog::add(int val)
{
    RBnode *cur = this->root;
    RBnode *n = new RBnode(val);
    int dir = 0;

    scene->addItem(n);

    while(cur != NULL) {
//        cur->select();
        dir = (cur->key > n->key) ? LEFT : RIGHT;

        if (cur->child[dir] == NULL)
            break;

//        cur->unSelect();
        cur = cur->child[dir];
    }

    n->parent = cur;

    if (cur == NULL) {
        this->root = n;
        n->setPos(scene->width() / 2, HGAP);
    }
    else {
        cur->child[dir] = n;
        n->setPos(cur->pos() + QPointF((2*dir-1) * WGAP, HGAP));

        Arrow *arrow = new Arrow(cur, n);
        cur->addArrow(arrow);
        n->addArrow(arrow);
        arrow->setZValue(-1000.0);
        scene->addItem(arrow);
        arrow->updatePosition();

        fixCollision(n);
//        fixRedRed(n, cur, dir);
    }
}

void Dialog::fixCollision(RBnode *n)
{
    RBnode* cur = n->parent;
    double nx = n->pos().x();

    //detect collision with ancestors
    while (cur != NULL)
    {
        if (nx == cur->pos().x())
            break;

        cur = cur->parent;
    }

    //collision not found
    if (cur == NULL)
        return;

    //collision with root
    if (cur == root)
    {
        //check wheter collision with right sub tree or left sub tree
        if (n->parent->pos().x() > root->pos().x())
            movePos(root->RIGHT_CHILD, WGAP, RIGHT);
        else
            movePos(root->LEFT_CHILD, WGAP, LEFT);

        return;
    }

    int aDir = CHILD_DIR(cur);      //ancestor direction
    int nDir = CHILD_DIR(n);        //node direction

    if (aDir == nDir)
    {
        cur->setPos(cur->pos() + QPointF(DIST_DIR(WGAP, aDir), 0));
        movePos(cur->child[aDir], WGAP, aDir);

        while(cur->child[aDir] != NULL)
            cur = cur->child[aDir];

        fixCollision(cur);
    }
    else
    {
       movePos(cur->child[aDir], WGAP, aDir);

       while(cur->child[aDir] != NULL)
           cur = cur->child[aDir];

       fixCollision(cur);
    }
}

void Dialog::movePos(RBnode *n, int dist, int dir)
{
    if (n == NULL)
        return;

    n->setPos(n->pos() + QPointF(DIST_DIR(dist, dir), 0));

    movePos(n->LEFT_CHILD, dist, dir);
    movePos(n->RIGHT_CHILD, dist, dir);
}

void Dialog::fixRedRed(RBnode *n, RBnode *p, int dir)
{
    struct RBnode* g;       // grand parent
    struct RBnode* u;       // uncle

//    n->color = RBnode::RED;
//    n->LEFT_CHILD  = NULL;
//    n->RIGHT_CHILD = NULL;
//    n->parent = p;

//    // emty tree
//    if (p == NULL) {
//        this->root = n;
//        return;
//    }

    p->child[dir] = n;
    do {
        // black parent
        if (p->color == RBnode::BLACK)
            return;

       // double red at root
        if ((g = p->parent) == NULL)
        {
            // change root to black, black height + 1
            p->color = RBnode::BLACK;
            return;
        }


        dir = CHILD_DIR(p);
        u = g->child[1-dir];

        // black parent and black uncle
        if (u == NULL || u->color == RBnode::BLACK)
        {
            // n is inner child
            if (n == p->child[1-dir])
            {
                ROTATE_DIR(p, dir);
                // now p is outer child of n
                //n = p;
                p = g->child[dir];
            }

            // n is outer child
            RotateDirRoot(g, 1-dir);
            p->color = RBnode::BLACK;
            g->color = RBnode::RED;
            return;
        }

        //red parent and red uncle
        p->color = RBnode::BLACK;
        u->color = RBnode::BLACK;
        g->color = RBnode::RED;
        n = g;      //grand parent might violate double red

    } while ((p = n->parent) != NULL);

    return;
}

RBnode* Dialog::RotateDirRoot(RBnode* p, int dir)
{

    RBnode* g = p->parent;
    RBnode* s = p->child[1-dir];
    RBnode* c;

    c = s->child[dir];
    p->child[1-dir] = c;
    if (c != NULL)
        c->parent = p;

    s->child[dir] = p;
    p->parent = s;
    s->parent = g;

    if (g != NULL)
        g->child[ p == g->RIGHT_CHILD ? RIGHT : LEFT ] = s;
    else
        this->root = s;

    return s; // new root of subtree
}

Dialog::~Dialog()
{
    delete ui;
}

