#ifndef _MY_DLX_H
#define _MY_DLX_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct Node {
    struct Node *left;
    struct Node *right;
    struct Node *up;
    struct Node *down;
    struct Node *column;
    int rowID;
    int colID;
    int nodeCount;
};

class DLX {
    
    int nCol, nRow;
    Node *header;
    vector< vector<Node> > matrix;

public:

    DLX(vector< vector<bool> >& p_);
    Node *leastOne(void);
    void cover(Node *);
    void uncover(Node *);
    bool solve(vector<Node *>&);

    int getRight(int i) { return (i + 1) % nCol; }
    int getLeft(int i) { return (i - 1 < 0) ? nCol - 1 : i - 1; }
    int getUp(int i) { return (i - 1 < 0) ? nRow - 1 : i - 1; }
    int getDown(int i) { return (i + 1) % nRow; }

};

#endif
