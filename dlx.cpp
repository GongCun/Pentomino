#include "dlx.hpp"

static int global;

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

bool DLX::solve() {
    // cout << "solve()" << endl;
    bool success = false;

    if (header->right == header) {
        print_solve(cout, solutions);
        // For testing in singleton mode
        if (++global == runs)
            exit(0);
        return true;
    }

    Node *column = leastOne();

    cover(column);

    for (Node *row = column->down; row != column; row = row->down) {
        solutions.push_back(row->rowID);

        for (Node *rightNode = row->right; rightNode != row;
             rightNode = rightNode->right)
            cover(rightNode);

        // if (solve()) return true;
        if (solve()) success = true;

        solutions.pop_back();
        column = row->column;
        for (Node *leftNode = row->left; leftNode != row;
             leftNode = leftNode->left)
            uncover(leftNode);
    }
    uncover(column);
    return success;
}

//
DLX::DLX(Node*& h, int& nCol_, int& nRow_, vector<int>& solutions_) :
    nCol(nCol_), nRow(nRow_),  solutions(solutions_) {

    header = new Node();

    matrix = vector< vector<Node> >(nRow, vector<Node>(nCol));
    // for (auto &v : matrix)
        // v = vector<Node>(nCol);

    for (Node *col = h->right; col != h; col = col->right) {
        Node *row = col;
        do {
            Node &n     = matrix[row->rowID][row->colID];
            n.nodeCount = row->nodeCount;
            n.rowID     = row->rowID;
            n.colID     = row->colID;
            n.column    = &matrix[0][row->colID];
            n.left      = &matrix[row->rowID][row->left->colID];
            n.right     = &matrix[row->rowID][row->right->colID];
            n.up        = &matrix[row->up->rowID][row->colID];
            n.down      = &matrix[row->down->rowID][row->colID];

            row         = row->down;
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
DLX::DLX(istream& in) {
    istreambuf_iterator<char> eos;
    string s(istreambuf_iterator<char>(in), eos);
    Document d;
    d.Parse(s.c_str());

    nCol = d["nCol"].GetInt();
    nRow = d["nRow"].GetInt();
    const Value& vs = d["solutions"];
    for (SizeType i = 0; i < vs.Size(); i++)
        solutions.push_back(vs[i].GetInt());

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


//
void distribute(unsigned k, DLX* root) {
    if (root->header->right == root->header)
        return;

    vector<DLX *> queue;

    queue.push_back(root);
    unsigned cur = 0, last = 1, level = 0;

    while (cur < queue.size()) {
        // cout << "level = " << level << endl;
        last = queue.size();

        auto t = last - cur;
        
        // cout << "level count = " << last - cur << endl;

        while (cur < last) {

            DLX* q = queue[cur++];
            if (t >= k) {
                // q->solve();
                dlxSerialize(cout, q);
                delete q;
                continue;
            }

            Node *column = q->leastOne();
            if (column == q->header) continue;

            q->cover(column);
            
            for (Node *row = column->down; row != column; row = row->down) {
                
                // cout << "cover " << row->rowID << endl;
                q->solutions.push_back(row->rowID);

                for (Node *rightNode = row->right; rightNode != row; rightNode = rightNode->right) {
                    q->cover(rightNode);
                }

                queue.push_back(new DLX(q->header, q->nCol, q->nRow, q->solutions));
                // cout << "queue.size: " << queue.size() << endl;

                q->solutions.pop_back();
                column = row->column;

                for (Node *leftNode = row->left; leftNode != row; leftNode = leftNode->left)
                    q->uncover(leftNode);
            }
            delete q;
        }
        level++;
    }
}

void dlxSerialize(ostream &o, DLX *dlx) {
    Document d;

    Pointer("/nCol").Set(d, dlx->nCol);
    Pointer("/nRow").Set(d, dlx->nRow);
    for (unsigned i = 0;  i < dlx->solutions.size(); i++) {
        string s = "/solutions/" + to_string(i);
        Pointer(s.c_str()).Set(d, dlx->solutions[i]);
    }

    Node *h = dlx->header;
    for (Node *col = h->right; col != h; col = col->right) {
        Node *row = col;
        do {
            string id = "/" + to_string(row->rowID) + "," + to_string(row->colID);
            // string token = "/matrix/" + id;
            
            Pointer((id + "/nodeCount").c_str()).Set(d, row->nodeCount);
            Pointer((id + "/rowID").c_str()).Set(d, row->rowID);
            Pointer((id + "/colID").c_str()).Set(d, row->colID);
            Pointer((id + "/left").c_str()).Set(d, row->left->colID);
            Pointer((id + "/right").c_str()).Set(d, row->right->colID);
            Pointer((id + "/up").c_str()).Set(d, row->up->rowID);
            Pointer((id + "/down").c_str()).Set(d, row->down->rowID);
            // Pointer("/matrix/row/nodeCount").Set(d, row->nodeCount);
            // Pointer("/rowID").Set(d, row->rowID);
            // Pointer("/colID").Set(d, row->colID);
            // Pointer("/left").Set(d, row->left->colID);
            // Pointer("/right").Set(d, row->right->colID);
            // Pointer("/up").Set(d, row->up->rowID);
            // Pointer("/down").Set(d, row->down->rowID);

            row = row->down;
        } while (row != col);
    }

    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    // Writer<StringBuffer> writer(sb);
    // printf("\nJSON string:\n");
    d.Accept(writer);
    // puts(sb.GetString());
    o << sb.GetString() << endl;
}
