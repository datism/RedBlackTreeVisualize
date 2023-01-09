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
    connect(ui->continueButton, &QPushButton::clicked, this, &Dialog::nextStep);

    root = NULL;
    next = false;

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

   textEdit->clear();
   return arr;
}

void Dialog::addNode()
{

    QVector<int> arr = readInput(ui->addTextEdit);

    for (auto v: arr)
    {
        add(v);
    }
}

void Dialog::deleteNode()
{
    QVector<int> arr = readInput(ui->deleteTextEdit);

    for (auto v: arr)
    {
        remove(v);
    }
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
                n = p;
                p = g->child[dir];

                ui->stepLabel->clear();
                updateAndWait();
                p->unSelect();
                n->select();

            }

            ui->stateLabel->setText("Red parent, black uncle, n is outer child");
            ui->stepLabel->setText("Step1/2: rotate grand parent, p become grandparent");
            updateAndWait();

            // n is outer child
            RotateDirRoot(g, 1-dir);

            ui->stepLabel->setText("Step2/2: p->black, s->red");
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

void Dialog::remove(int val)
{
    RBnode *n = findNode(val);
    if (n == NULL)
        return;

    if (n == this->root && n->LEFT_CHILD == NULL && n->RIGHT_CHILD == NULL)
    {
        ui->stateLabel->setText("Removed node is root and doesnt have any child");
        ui->stepLabel->setText("Step1/1: tree is null");
        updateAndWait();

        scene->removeItem(n);
        delete n;
        this->root = NULL;

        ui->stepLabel->clear();
        updateAndWait();
        ui->stateLabel->clear();

        return;
    }

    RBnode *r = findReplace(n);

    ui->stateLabel->setText("Swap node key");
    r->select();
    n->select();
    updateAndWait();

    swapValue(r, n);

    updateAndWait();
    r->unSelect();
    n->unSelect();
    ui->stateLabel->clear();

    if (r->color == RBnode::RED)
    {
        ui->stateLabel->setText("Removed node is red");
        ui->stepLabel->setText("Step1/1: Just remove node from tree");
        r->select();
        updateAndWait();

        removeArrow(r, PARENT);
        r->parent->child[CHILD_DIR(r)] = NULL;
        scene->removeItem(r);
        delete r;

        ui->stepLabel->clear();
        updateAndWait();
        ui->stateLabel->clear();

        return;
    }

    RBnode *c = (r->RIGHT_CHILD != NULL) ? r->RIGHT_CHILD : r->LEFT_CHILD;
    if (c != NULL) {
        ui->stateLabel->setText("Removed node is black and it has 1 child");
        ui->stepLabel->setText("Step1/2: Swap key n with c");
        r->select();
        updateAndWait();

        swapValue(r, n);

        ui->stepLabel->setText("Step2/2: Remove c");
        updateAndWait();

        removeArrow(c, PARENT);
        scene->removeItem(c);
        delete c;

        r->unSelect();
        ui->stepLabel->clear();
        updateAndWait();
        ui->stateLabel->clear();

        return;
    }



    fixDoubleBlack(r);

    scene->removeItem(r);
    delete r;
}

void Dialog::fixDoubleBlack(RBnode *n)
{
    struct RBnode* p = n->parent;
    struct RBnode* s;  // sibling of n
    struct RBnode* c;  // close nephew
    struct RBnode* d;  // distant nephew
    int dir;

    removeArrow(n, PARENT);
    movePos(n, QPointF(0, HGAP));

    dir = CHILD_DIR(n);
    p->child[dir] = NULL;

    do {
        n->select();

        if (p->child[dir] != NULL)
            dir = CHILD_DIR(n);

        s = p->child[1-dir];
        d = s->child[1-dir];
        c = s->child[ dir];

        // s red ===> p+c+d black
        if (s->color == RBnode::RED)
        {
            ui->stateLabel->setText("Sibling is red");
            ui->stepLabel->setText("Step1/2: rotate p, s become parent");
            updateAndWait();

            RotateDirRoot(p, dir);

            ui->stepLabel->setText("Step2/2: p->black, s->red");
            updateAndWait();

            p->color = RBnode::RED;
            s->color = RBnode::BLACK;

            s = c;
            if (s != NULL)
            {
                d = s->child[1-dir];
                c = s->child[dir];
            }
            // now p red && s black
        }

        // c red && s+d black
        if (c != NULL && c->color == RBnode::RED)
        {
            ui->stateLabel->setText("Close nephew is red");
            ui->stepLabel->setText("Step1/2: rotate s, c become sibling");
            updateAndWait();

            ROTATE_DIR(s, 1-dir);

            ui->stepLabel->setText("Step2/2: s->black, d->red");
            updateAndWait();

            s->color = RBnode::RED;
            c->color = RBnode::BLACK;

            d = s;
            s = c;
            // now: d red && s black
        }

        // d red && s+c black
        if (d != NULL && d->color == RBnode::RED)
        {
            ui->stateLabel->setText("Distant nephew is red");
            ui->stepLabel->setText("Step1/2: rotate p, s become parent");
            updateAndWait();

            RotateDirRoot(p, dir);

            ui->stepLabel->setText("Step2/2: g->p color, uncle->black, p->black");
            updateAndWait();

            s->color = p->color;
            p->color = RBnode::BLACK;
            d->color = RBnode::BLACK;

            break;
        }

        // p red && c+s+d black
        if (p->color == RBnode::RED)
        {
            ui->stateLabel->setText("Parent is red");
            ui->stepLabel->setText("Step1/1: p->black, s->red");
            updateAndWait();

            s->color = RBnode::RED;
            p->color = RBnode::BLACK;

            break;
        }

        // p + s + c + d black
        ui->stateLabel->setText("p, s, c, d black");
        ui->stepLabel->setText("Step1/2: s->red");
        updateAndWait();

        s->color = RBnode::RED;

        ui->stepLabel->setText("Step2/2: check violation for p");
        updateAndWait();

        n->unSelect();
        n = p;
    } while ((p = n->parent) != NULL);

    ui->stepLabel->clear();
    updateAndWait();
    ui->stateLabel->clear();
    return;
}

RBnode* Dialog::findNode(int val)
{
    RBnode *cur = this->root;

    ui->stateLabel->setText("Find node");
    while (cur != NULL)
    {
        cur->select();

        updateAndWait();
        cur->unSelect();

        if (cur->key == val)
            return cur;

        cur = (cur->key > val) ? cur->LEFT_CHILD : cur->RIGHT_CHILD;
    }

    ui->stateLabel->clear();
    return cur;
}

RBnode* Dialog::findReplace(RBnode *n)
{
    RBnode *cur = n;
    int dir;

    ui->stateLabel->setText("Find replace node");
    cur->select();

    if (cur->LEFT_CHILD != NULL || cur->RIGHT_CHILD != NULL)
    {
        if (cur->LEFT_CHILD == NULL)
            dir = RIGHT;
        else
            dir = LEFT;

        cur = cur->child[dir];
        while (cur->child[1-dir] != NULL)
        {
            cur->select();

            updateAndWait();
            cur->unSelect();

            cur = cur->child[1-dir];
        }
    }

    cur->unSelect();
    ui->stateLabel->clear();
    return cur;
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

void Dialog::nextStep()
{
    next = true;
}

void Dialog::updateAndWait()
{
    next = false;
    ui->addButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    while (1)
    {
        if (next)
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

