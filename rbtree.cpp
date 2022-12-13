#include "rbtree.h"
#include <iostream>

RBnode::RBnode(int i_key)
    : key(i_key), parent(NULL), color(RED)
{
    RIGHT_CHILD = NULL;
    LEFT_CHILD = NULL;
}

RBtree::RBtree()
    :root(NULL)
{}

RBnode* RBtree::RotateDirRoot(RBnode* p, int dir)
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

void RBtree::fixRedRed(RBnode *n, RBnode *p, int dir)
{
    struct RBnode* g;       // grand parent
    struct RBnode* u;       // uncle

    n->color = RBnode::RED;
    n->LEFT_CHILD  = NULL;
    n->RIGHT_CHILD = NULL;
    n->parent = p;

    // emty tree
    if (p == NULL) {
        this->root = n;
        return;
    }

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

void RBtree::add(int val)
{
    RBnode *cur = this->root;
    RBnode *n = new RBnode(val);
    int dir = 0;

    while(cur != NULL) {
        dir = (cur->key > n->key) ? LEFT : RIGHT;

        if (cur->child[dir] == NULL)
            break;

        cur = cur->child[dir];
    }

    fixRedRed(n, cur, dir);
}

//void RBtree::fixDoubleBlack(RBnode *n)
//{
//    struct RBnode* p = n->parent;
//    struct RBnode* s;  // sibling of n
//    struct RBnode* c;  // close nephew
//    struct RBnode* d;  // distant nephew
//    int dir;

//    dir = CHILD_DIR(n);
//    p->child[dir] = NULL;

//    do {
//        if (p->child[dir] != NULL)
//            dir = CHILD_DIR(n);

//        s = p->child[1-dir];
//        d = s->child[1-dir];
//        c = s->child[ dir];

//        // s red ===> p+c+d black
//        if (s->color == RBnode::RED)
//        {
//            RotateDirRoot(p, dir);
//            p->color = RBnode::RED;
//            s->color = RBnode::BLACK;
//            s = c;
//            d = s->child[1-dir];
//            c = s->child[dir];
//            // now p red && s black
//        }

//        // c red && s+d black
//        if (c != NULL && c->color == RBnode::RED)
//        {
//            ROTATE_DIR(s, 1-dir);
//            s->color = RBnode::RED;
//            c->color = RBnode::BLACK;
//            d = s;
//            s = c;
//            // now: d red && s black
//        }

//        // d red && s+c black
//        if (d != NULL && d->color == RBnode::RED)
//        {
//            RotateDirRoot(p, dir);
//            s->color = p->color;
//            p->color = RBnode::BLACK;
//            d->color = RBnode::BLACK;
//            return;
//        }

//        // p red && c+s+d black
//        if (p->color == RBnode::RED)
//        {
//            s->color = RBnode::RED;
//            p->color = RBnode::BLACK;
//            return;
//        }

//        // p + c + d black
//        s->color = RBnode::RED;
//        n = p;
//    } while ((p = n->parent) != NULL);

//    return;
//}

//RBnode* RBtree::find(int val)
//{
//    RBnode *cur = this->root;

//    while (cur != NULL)
//    {
//        if (cur->key == val)
//            return cur;

//        cur = (cur->key > val) ? cur->LEFT_CHILD : cur->RIGHT_CHILD;
//    }

//    return cur;
//}

//RBnode* RBtree::findReplace(RBnode *n)
//{
//    RBnode *cur = n;
//    int dir;

//    if (cur->LEFT_CHILD == NULL && cur->RIGHT_CHILD == NULL)
//        return n;
//    else if (cur->LEFT_CHILD == NULL)
//        dir = RIGHT;
//    else
//        dir = LEFT;

//    cur = cur->child[dir];
//    while (cur->child[1-dir] != NULL)
//        cur = cur->child[1-dir];

//    return cur;
//}

//void RBtree::remove(int val)
//{
//    RBnode *n = find(val);
//    if (n == NULL)
//        return;

//    if (n == this->root && n->LEFT_CHILD == NULL && n->RIGHT_CHILD == NULL)
//    {
//        this->root = NULL;
//        return;
//    }

//    RBnode *r = findReplace(n);
//    swap(r, n);

//    if (r->color == RBnode::RED)
//    {
//        r->parent->child[CHILD_DIR(r)] = NULL;
//        delete r;
//        return;
//    }

//    RBnode *c = (r->RIGHT_CHILD != NULL) ? r->RIGHT_CHILD : r->LEFT_CHILD;
//    if (c != NULL) {
//        swap(r, n);
//        delete c;
//        return;
//    }

//    fixDoubleBlack(r);
//    delete r;
//}
