#ifndef RBTREE_H
#define RBTREE_H

#include <cstddef>

#define LEFT  0
#define RIGHT 1
#define LEFT_CHILD  child[LEFT]
#define RIGHT_CHILD child[RIGHT]
#define CHILD_DIR(n) ( n == (n->parent)->RIGHT_CHILD ? RIGHT : LEFT )

#define ROTATE_DIR(n,dir) RotateDirRoot(n,dir)
#define ROTATE_LEFT(n)    RotateDirRoot(n,LEFT)
#define ROTATE_RIGHT(n)   RotateDirRoot(n,RIGHT)


class RBnode {
private:
    enum Color { BLACK, RED };
    RBnode* parent;
    RBnode* child[2];
    int key;
    Color color;
public:
    RBnode(int i_key);

    friend class RBtree;
    friend void swap(RBnode *a, RBnode *b);
};

class RBtree { // red–black tree
private:
    RBnode* RotateDirRoot(RBnode* p, int dir);
    void fixRedRed(RBnode* n, RBnode* p, int dir);
    void fixDoubleBlack(RBnode *n);
    RBnode* find(int val);
    RBnode* findReplace(RBnode* n);

public:
    RBtree();
    RBnode* root; // == NULL if tree is empty
    void add(int val);
    void remove(int val);
};


#endif // RBTREE_H
