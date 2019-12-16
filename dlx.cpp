#include "dlx.hpp"

DLX::DLX(vector< vector<bool> >& p_) {

    nRow = p_.size();
    nCol = p_[0].size();
    header = new Node();
    
    matrix = vector< vector<Node> >(p_.size());
    for (auto &v : matrix)
        v = vector<Node>(nCol);

    for (auto i = 0; i < nRow; i++) {
        for (auto j = 0; j < nCol; j++) {
            if (p_[i][j]) {
                int a, b;
                if (i) matrix[0][j].nodeCount += 1;

                matrix[i][j].column = &matrix[0][j];
                matrix[i][j].rowID = i;
                matrix[i][j].colID = j;

                // Link the node with neighbors

                // Left pointer
                a = i, b = j;
                do {
                    b = getLeft(b);
                } while (!p_[a][b] && b != j);
                matrix[i][j].left = &matrix[i][b];

                // Right pointer
                a = i, b = j;
                do {
                    b = getRight(b);
                } while (!p_[a][b] && b != j);
                matrix[i][j].right = &matrix[i][b];

                // Up pointer
                a = i, b = j;
                do {
                    a = getUp(a);
                } while (!p_[a][b] && a != i);
                matrix[i][j].up = &matrix[a][j];

                // Down pointer
                a = i, b = j;
                do {
                    a = getDown(a);
                } while (!p_[a][b] && a != i);
                matrix[i][j].down = &matrix[a][j];
            }
        }
    }

    // Link header right pointer to column header of first column
    header->right = &matrix[0][0];

    // Link header left pointer to column header of last column
    header->left = &matrix[0][nCol - 1];

    matrix[0][0].left = header;
    matrix[0][nCol - 1].right = header;

}

Node *DLX::leastOne(void) {
    Node *h = header;
    Node *min = h->right;
    h = h->right->right;
    do {
        // if (h->nodeCount > 0 && h->nodeCount < min->nodeCount)
        if (h->nodeCount < min->nodeCount)
            min = h;
        h = h->right;
    } while (h != header);

    return min;
}


// Cover the given node completely
void DLX::cover(Node *targetNode) {

    // get the pointer to the header of column to which this node belong.
    Node *colNode = targetNode->column;

    // unlink column header from it's neighbors
    colNode->left->right = colNode->right;
    colNode->right->left = colNode->left;

    // Move down the column and remove each row by traversing right
    for (Node *row = colNode->down; row != colNode; row = row->down) {
        for (Node *rightNode = row->right; rightNode != row;
             rightNode = rightNode->right) {
            rightNode->up->down = rightNode->down;
            rightNode->down->up = rightNode->up;

            matrix[0][rightNode->colID].nodeCount -= 1;
        }
    }
}

// Uncover the given node completely
void DLX::uncover(Node *targetNode) {
    Node *colNode = targetNode->column;

    for (Node *row = colNode->up; row != colNode; row = row->up) {
        for (Node *leftNode = row->left; leftNode != row;
             leftNode = leftNode->left) {
            leftNode->up->down = leftNode;
            leftNode->down->up = leftNode;

            matrix[0][leftNode->colID].nodeCount += 1;
        }
    }

    // Link the column header from it's neighbors
    colNode->left->right = colNode;
    colNode->right->left = colNode;
}

// void DLX::print(ostream &o) const {
//     for (auto &v : solutions)
//         o << v->rowID << " ";
//     o << endl;
// }

bool DLX::solve() {
    if (header->right == header) {
        // if (solutions.size() == 81)
        //     return true;
        // else
        //     return false;
        return true;
    }

    Node *column = leastOne();
    if (column == header)
        return false;

    cover(column);

    for (Node *row = column->down; row != column; row = row->down) {
        solutions.push_back(row->rowID);

        for (Node *rightNode = row->right; rightNode != row;
             rightNode = rightNode->right)
            cover(rightNode);

        if (solve()) return true;

        solutions.pop_back();
        column = row->column;
        for (Node *leftNode = row->left; leftNode != row;
             leftNode = leftNode->left)
            uncover(leftNode);
    }
    uncover(column);
    return false;
}

//
DLX::DLX(Node *h, int nCol_, int nRow_, vector<int>& solutions_) :
    nCol(nCol_), nRow(nRow_),  solutions(solutions_) {

    header = new Node();

    matrix = vector< vector<Node> >(nRow);
    for (auto &v : matrix)
        v = vector<Node>(nCol);

    for (Node *col = h->right; col != h; col = col->right) {
        Node *row = col;
        do {
            Node &n  = matrix[row->rowID][row->colID];
            n.column = &matrix[0][row->colID];
            n.rowID  = row->rowID;
            n.colID  = row->colID;
            n.left   = &matrix[row->rowID][row->left->colID];
            n.right  = &matrix[row->rowID][row->right->colID];
            n.up     = &matrix[row->up->rowID][row->colID];
            n.down   = &matrix[row->down->rowID][row->colID];

            row = row->down;
        } while (row != col);
    }

    if (h->right != h) {
        header->right = &matrix[0][h->right->colID];
        header->left = &matrix[0][h->left->colID];
        matrix[0][h->right->colID].left = header;
        matrix[0][h->left->colID].right = header;
    } else {
        header->right = header;
        header->left = header;
    }

}


//
void DLX::print() {
    cout << "print start" << endl;
    for (Node *col = header->right; col != header; col = col->right) {
        for (Node *row = col->down; row != col; row = row->down)
            cout << row->rowID << ", " << row->colID << endl;
    }
    cout << "print end" << endl;
}

static void print_solve(vector<int>& s) {
    if (!s.empty()) {
        for (auto &v : s)
            cout << v << " ";
        cout << endl;
    } else {
        cout << "solutions is empty" << endl;
    }
}

//
void distribute(unsigned k, DLX* root) {
    if (root->header->right == root->header)
        return;

    vector<DLX *> queue;

    queue.push_back(root);
    unsigned cur = 0, last = 1, level = 0;

    while (cur < queue.size()) {
        cout << "level = " << level << endl;
        last = queue.size();

        while (cur < last) {

            DLX*& q = queue[cur++];
            DLX dlx(q->header, q->nCol, q->nRow, q->solutions);
            delete q;

            if (level == k) {
                // cout << ">> initial solutions: ";
                // print_solve(dlx.solutions);
                // dlx.print();

                if (dlx.solve()) {
                    cout <<"final solutions: ";
                    print_solve(dlx.solutions);
                } else {
                    cout <<"no solutions" << endl;
                }
                continue;
            }

            Node *column = dlx.leastOne();
            if (column == dlx.header) continue;

            dlx.cover(column);
            
            for (Node *row = column->down; row != column; row = row->down) {
                
                // cout << "cover " << row->rowID << endl;
                dlx.solutions.push_back(row->rowID);

                for (Node *rightNode = row->right; rightNode != row; rightNode = rightNode->right) {
                    dlx.cover(rightNode);
                }

                queue.push_back(new DLX(dlx.header, dlx.nCol, dlx.nRow, dlx.solutions));

                dlx.solutions.pop_back();
                column = row->column;

                for (Node *leftNode = row->left; leftNode != row; leftNode = leftNode->left)
                    dlx.uncover(leftNode);
            }
        }
        level++;
    }
}
