#include <bits/stdc++.h>
using namespace std;

template<typename U, typename T>  //U : key, T : value
class RBTree
{
    enum Color {RED, BLACK};
    struct node
    {
        node* left, *right, *father;
        Color color;
        U key;
        T value;
        node(const U &_key, const T &_value) : left(nullptr), right(nullptr), father(nullptr), color(RED), key(_key), value(_value) {}
        node(Color c, const U &_key, const T &_value) : left(nullptr), right(nullptr), father(nullptr), color(c), key(_key), value(_value) {}
    };
public:
    node *root;
    
public:
    RBTree() : root(nullptr) {}

private:
    void recurClear(node *t)
    {
        if (t == nullptr) return;
        recurClear(t -> left);
        recurClear(t -> right);
        delete t;
    }
public:
    void clear()
    {
        recurClear(root);
        delete root;
        root = nullptr;
    }
    ~RBTree()
    {
        clear();
    }

private:
    bool isLeftSon(node *t)
    {
        if (t == nullptr) return true;
        return t -> father != nullptr && t -> father -> left == t;
    }

    Color getColor(node *t)
    {
        if (t == nullptr || t -> color == BLACK) return BLACK;
        else return RED;
    }

private:
    void leftRotate(node *x)
    {
        node *y = x -> right;
        x -> right = y -> left;
        if (y -> left != nullptr) y -> left -> father = x;
        y -> father = x -> father;
        if (x -> father == nullptr) root = y;
        else if (isLeftSon(x)) x -> father -> left = y;
        else x -> father -> right = y;
        y -> left = x;
        x -> father = y;
    }

    void rightRotate(node *x)
    {
        node *y = x -> left;
        x -> left = y -> right;
        if (y -> right != nullptr) y -> right -> father = x;
        y -> father = x -> father;
        if (x -> father == nullptr) root = y;
        else if (isLeftSon(x)) x -> father -> left = y;
        else x -> father -> right = y;
        y -> right = x;
        x -> father = y;
    }

private:
    void insertFixup(node *z)
    {
        while (getColor(z -> father) == RED)
        {
            if (isLeftSon(z -> father))
            {
                node *y = z -> father -> father -> right;
                if (getColor(y) == RED) //uncle is red
                {
                    z -> father -> color = BLACK;
                    y -> color = BLACK;
                    z -> father -> father -> color = RED;
                    z = z -> father -> father;
                }
                else if (isLeftSon(z)) //uncle is black and z is left son
                {
                    z -> father -> color = BLACK;
                    z -> father -> father -> color = RED;
                    rightRotate(z -> father -> father);
                }
                else //uncle is black and z is right son
                {
                    z = z -> father;
                    leftRotate(z);
                }
            }
            else //reflection of the other case
            {
                node *y = z -> father -> father -> left;
                if (getColor(y) == RED)
                {
                    z -> father -> color = BLACK;
                    y -> color = BLACK;
                    z -> father -> father -> color = RED;
                    z = z -> father -> father;
                }
                else if (!isLeftSon(z))
                {
                    z -> father -> color = BLACK;
                    z -> father -> father -> color = RED;
                    leftRotate(z -> father -> father);
                }
                else
                {
                    z = z -> father;
                    rightRotate(z);
                }
            }
        }
        root -> color = BLACK;
    }
    
    void eraseFixup(node *x, node *parent)
    {
        //cout << x <<" "<<parent<<endl;
        node *w;
        while (x != root && getColor(x) == BLACK)
        {
            //cout << 1 << " "<<isLeftSon(x)<<endl;
            if (isLeftSon(x))
            {
                w = parent -> right;
                if (getColor(w) == RED)
                {
                    w -> color = BLACK;
                    parent -> color = RED;
                    leftRotate(parent);
                    w = parent -> right;
                }
                else if (w == nullptr || (getColor(w -> left) == BLACK && getColor(w -> right) == BLACK))
                {
                    if (w) w -> color = RED;
                    x = parent;
                    parent = parent -> father;
                }
                else if (getColor(w -> left) == RED && getColor(w -> right) == BLACK)
                {
                    w -> left -> color = BLACK;
                    w -> color = RED;
                    rightRotate(w);
                    w = parent -> right;
                }
                else if (getColor(w -> right) == RED)
                {
                    w -> color = parent -> color;
                    parent -> color = BLACK;
                    w -> right -> color = BLACK;
                    leftRotate(parent);
                    x = root;
                    break;
                }
            }
            else
            {
                w = parent -> left;
                if (getColor(w) == RED)
                {
                    w -> color = BLACK;
                    parent -> color = RED;
                    rightRotate(parent);
                    w = parent -> left;
                }
                else if (getColor(w -> left) == BLACK && getColor(w -> right) == BLACK)
                {
                    w -> color = RED;
                    x = parent;
                    parent = parent -> father;
                }
                else if (getColor(w -> right) == RED && getColor(w -> left) == BLACK)
                {
                    w -> right -> color = BLACK;
                    w -> color = RED;
                    leftRotate(w);
                    w = parent -> left;
                }
                else if (getColor(w -> left) == RED)
                {
                    w -> color = parent -> color;
                    parent -> color = BLACK;
                    w -> left -> color = BLACK;
                    rightRotate(parent);
                    x = root;
                    break;
                }
            }
        }
        if (x != nullptr) x -> color = BLACK;
    }

public:
    node* findNode(U key) 
    {
        if (root == nullptr) return nullptr;
        node *x = root;
        while (x != nullptr)
        {
            //cout << x->key << endl;
            if (key == x -> key)
            {
                return x;
            }
            if (key < x -> key) x = x -> left;
            else x = x -> right;
        }
        return nullptr;
    }
    
    void insert(U key, T value)
    {
        node *z = new node(RED, key, value); 
        node *x = root, *y = nullptr;
        while (x != nullptr)
        {
            y = x;
            if (key < x -> key) x = x -> left;
            else x = x -> right;
        }
        z -> father = y;
        if (y == nullptr) root = z;
        else if (key < (y -> key)) y -> left = z;
        else y -> right = z;
        insertFixup(z);
    }
    
    void erase(U key)
    {
        node *z = findNode(key), *x, *y;
        if (z -> right == nullptr || z -> left == nullptr) y = z;
        else
        {
            y = z -> right;
            while (y -> left != nullptr) y = y -> left;
        }
        
        if (y -> left != nullptr) x = y -> left;
        else x = y -> right;
        if (x != nullptr) x -> father = y -> father;
        
        if (y -> father == nullptr) root = x; //y is the root
        else if (isLeftSon(y)) y -> father -> left = x;
        else y -> father -> right = x;

        if (y != z) //the case where y is the successor of z
        {
            z -> key = y -> key;
            z -> value = y -> value; //copy the key & value, but not the color
        }
        if (y -> color == BLACK)
        {
            eraseFixup(x, y -> father);
        }
    }
};

int main()
{
    RBTree<int, int> rbt;
    for (int i=1;i<=10000000;i++) rbt.insert(i,i + 12345);
    for (int i=1;i<=10000000;i++) assert(rbt.findNode(i)->value == i + 12345);
    for (int i=1;i<=10000000;i++) rbt.erase(i);
    for (int i=1;i<=10000000;i++) rbt.insert(i,i + 54321);
    for (int i=1;i<=10000000;i++) assert(rbt.findNode(i)->value == i + 54321);
    return 0;
}

