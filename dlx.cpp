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
        if (h->nodeCount && h->nodeCount < min->nodeCount)
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

bool DLX::solve(vector<int>& solutions) {
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

        if (solve(solutions)) return true;

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
vector< vector<bool> > DLX::dlx2possible() {
    vector< vector<bool> >p(nRow, vector<bool>(nCol, false));
    p[0] = vector<bool>(nCol, true);
    
    // Node *h = header;
    for (Node *col = header->right; col != header; col = col->right) {
        // p[0][col->colID] = true;
        for (Node *row = col->down; row != col; row = row->down)
            p[row->rowID][row->colID] = true;
    }

    return p;
}

//
void DLX::print() {
    for (Node *col = header->right; col != header; col = col->right) {
        for (Node *row = col->down; row != col; row = row->down)
            // p[row->rowID][row->colID] = true;
            cout << row->rowID << ", " << row->colID << endl;
    }
}

static void print_solve(vector<int>& s) {
    for (auto &v : s)
        cout << v << " ";
    cout << endl;
}

//
void DLX::distribute(unsigned k) {
    if (header->right == header)
        return;

    vector<Qnode> queue;
    vector<int> solutions;

    Qnode qnode = {
        .possible_ = dlx2possible(),
        .solutions_ = solutions
    };


    queue.push_back(qnode);
    unsigned cur = 0, last = 1, level = 0;

    while (cur < queue.size()) {
        cout << "a level = " << level << endl;
        last = queue.size();

        while (cur < last) {
            Qnode &q = queue[cur++];
            DLX dlx(q.possible_);

            if (level == k) {
                cout << "start" << endl;

                cout << ">> ";
                for (auto &v : q.solutions_)
                    cout << v << " ";
                cout << endl;
                
                dlx.print();
                if (dlx.solve(q.solutions_))
                    cout << "!! success" << endl;
                
                    // print_solve(q.solutions_);

                cout << "end" << endl;
                continue;
            }

            Node *column = dlx.leastOne();
            if (column == dlx.header) continue;
            // for (Node *h = dlx.header->right; h != dlx.header; h = h->right) {
            //     cout << "h: " << h << " count: " << h->nodeCount << endl;
            // }
            
            // cout << "b level = " << level << endl;
            
            dlx.cover(column);
            // cout << column << " " << column << endl;
            // cout << "count " << column->nodeCount << endl;
            
            for (Node *row = column->down; row != column; row = row->down) {
                // cout << "hello" << endl;
                
                vector<int> s(q.solutions_);
                cout << "cover " << row->rowID << endl;
                s.push_back(row->rowID);

                for (Node *rightNode = row->right; rightNode != row; rightNode = rightNode->right) {
                    // cout << "cover " << rightNode->rowID << ", " << rightNode->colID << endl;
                    dlx.cover(rightNode);
                }

                Qnode qnode1 = {
                    .possible_ = dlx.dlx2possible(),
                    .solutions_ = s
                };
                queue.push_back(qnode1);

                column = row->column;

                for (Node *leftNode = row->left; leftNode != row; leftNode = leftNode->left)
                    dlx.uncover(leftNode);
            }

            // cur++;
        }
        level++;
        // if (++level > k) return;
    }
}
