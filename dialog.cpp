#include "dialog.h"
#include "./ui_dialog.h"
#include "arrow.h"

#include <QScreen>
#include <QPushButton>
#include <QThread>

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
    connect(ui->continueButton, &QPushButton::clicked, this, &Dialog::con);

    root = NULL;
    wait = false;

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

    ui->stateLabel->setText("Find pos for new node");
    while(cur != NULL) {
        cur->select();
        dir = (cur->key > n->key) ? LEFT : RIGHT;

        updateAndWait();
        cur->unSelect();
        if (cur->child[dir] == NULL)
            break;

        cur = cur->child[dir];
    }

    n->parent = cur;

    if (cur == NULL) {
        this->root = n;
        n->setPos(scene->width() / 2, HGAP);
        ui->stateLabel->clear();
    }
    else {
        cur->child[dir] = n;
        n->setPos(cur->pos() + QPointF(DIST_DIR(WGAP,dir), HGAP));
        addArrow(cur, n, dir);
        fixCollision(n);
        fixRedRed(n, cur, dir);
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
            movePos(root->RIGHT_CHILD, QPointF(DIST_DIR(WGAP,RIGHT), 0));
        else
            movePos(root->LEFT_CHILD, QPointF(DIST_DIR(WGAP,LEFT), 0));

        return;
    }

    int aDir = CHILD_DIR(cur);      //ancestor direction
    int nDir = CHILD_DIR(n);        //node direction

    if (aDir == nDir)
        cur->setPos(cur->pos() + QPointF(DIST_DIR(WGAP, aDir), 0));
    movePos(cur->child[aDir], QPointF(DIST_DIR(WGAP,aDir), 0));

    while(cur->child[aDir] != NULL)
       cur = cur->child[aDir];

    fixCollision(cur);
}

void Dialog::movePos(RBnode *n, const QPointF &dist)
{
    if (n == NULL)
        return;

    n->setPos(n->pos() + dist);

    movePos(n->LEFT_CHILD, dist);
    movePos(n->RIGHT_CHILD, dist);
}

void Dialog::fixRedRed(RBnode *n, RBnode *p, int dir)
{
    struct RBnode* g;       // grand parent
    struct RBnode* u;       // uncle

    p->child[dir] = n;
    do {
        n->select();

        // black parent
        if (p->color == RBnode::BLACK)
        {
            ui->stateLabel->setText("Black parent");
            ui->stepLabel->setText("no violation");

            updateAndWait();
            break;
        }

       // double red at root
        if ((g = p->parent) == NULL)
        {
            ui->stateLabel->setText("Red parent and parent is root");
            ui->stepLabel->setText("Step1/1: change root to black");
            updateAndWait();

            // change root to black, black height + 1
            p->color = RBnode::BLACK;
            break;
        }


        dir = CHILD_DIR(p);
        u = g->child[1-dir];

        // Red parent and black uncle
        if (u == NULL || u->color == RBnode::BLACK)
        {
            // n is inner child
            if (n == p->child[1-dir])
            {
                ui->stateLabel->setText("Red parent, black uncle, n is inner child");
                ui->stepLabel->setText("Step1/1: rotate parent, n become parent");
                updateAndWait();

                ROTATE_DIR(p, dir);
                // now p is outer child of n
                p = g->child[dir];

                ui->stepLabel->clear();
                updateAndWait();
            }

            ui->stateLabel->setText("Red parent, black uncle, n is outer child");
            ui->stepLabel->setText("Step1/2: rotate grand parent, p become grandparent");
            updateAndWait();

            // n is outer child
            RotateDirRoot(g, 1-dir);

            ui->stepLabel->setText("Step2/2: change color of parent and sibling");
            updateAndWait();

            p->color = RBnode::BLACK;
            g->color = RBnode::RED;
            break;
        }


        //red parent and red uncle
        ui->stateLabel->setText("Red parent and red uncle");
        ui->stepLabel->setText("Step1/2: p->black, u->black, g->red");
        updateAndWait();

        p->color = RBnode::BLACK;
        u->color = RBnode::BLACK;
        g->color = RBnode::RED;

        ui->stepLabel->setText("Step2/2: check violation for g");
        updateAndWait();
        n->unSelect();
        n = g;      //grand parent might violate double red

    } while ((p = n->parent) != NULL);

    ui->stepLabel->clear();
    updateAndWait();
    n->unSelect();
    ui->stateLabel->clear();
    return;
}

RBnode* Dialog::RotateDirRoot(RBnode* p, int dir)
{

    RBnode* g = p->parent;
    RBnode* s = p->child[1-dir];
    RBnode* c = s->child[dir];

    if (g != NULL)
        removeArrow(p, PARENT);
    removeArrow(s, PARENT);
    if (c != NULL)
        removeArrow(c, PARENT);

    p->child[1-dir] = NULL;
    movePos(p, QPointF(0, HGAP));

    p->child[1-dir] = c;
    if (c != NULL) {
        c->parent = p;
        addArrow(p, c, 1-dir);
    }

    s->child[dir] = NULL;
    movePos(s, QPointF(0, -HGAP));

    s->child[dir] = p;
    p->parent = s;
    s->parent = g;

    addArrow(s, p, dir);

    if (g != NULL)
    {
        dir = (p == g->RIGHT_CHILD) ? RIGHT : LEFT;
        g->child[dir] = s;
        addArrow(g, s, dir);
    }
    else
        this->root = s;

    return s; // new root of subtree
}

void Dialog::addArrow(RBnode *p, RBnode *n, int d)
{
    Arrow *arrow = new Arrow(p, n);
    p->arrows[d] = arrow;
    n->arrows[PARENT] = arrow;
    arrow->setZValue(-1000.0);
    scene->addItem(arrow);
    arrow->updatePosition();
}

void Dialog::removeArrow(RBnode *n, int i)
{
    Arrow *arrow = n->arrows[i];
    n->arrows[i] = NULL;

    if (i == PARENT)
        n->parent->arrows[CHILD_DIR(n)] = NULL;
    else
        n->child[i]->arrows[PARENT] = NULL;

    scene->removeItem(arrow);
    delete arrow;
}

void Dialog::con()
{
    wait = true;
}

void Dialog::updateAndWait()
{
    wait = false;
    ui->addButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    while (1)
    {
        if (wait)
            break;
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
    ui->addButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
}

Dialog::~Dialog()
{
    delete ui;
}

