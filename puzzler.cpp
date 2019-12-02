#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <memory>
#include <unistd.h>
#include <stdlib.h>
using namespace std;

const int ROW = 5;
const int COL = 11;

int nRow;
int nCol;
int global, runs;

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

class Position {
public:
    int x, y;
    Position() : x(0), y(0) {}
    Position(int x_, int y_) : x(x_), y(y_) {}

    friend ostream & operator << (ostream &o, const Position &p);
};

inline ostream & operator << (ostream &o, const Position &p) {
    o << p.x << ", " << p.y;
    return o;
}

map<unsigned, char> cell;
map<unsigned, char> sn; // name of shapes
vector< vector<bool> > p_;
vector<Node *> solutions;
vector< vector<Node> > Matrix;
Node *header;
Node *least_one(void);
Node *createNodeMatrix();
void cover(Node *);
void uncover(Node *);
void solve(void);

int getRight(int i) { return (i + 1) % nCol; }
int getLeft(int i) { return (i - 1 < 0) ? nCol - 1 : i - 1; }
int getUp(int i) { return (i - 1 < 0) ? nRow - 1 : i - 1; }
int getDown(int i) { return (i + 1) % nRow; }



Node *least_one(void) {
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

#if 0
inline ostream & operator << (ostream &o, const Possible &p) {
    cout << "total: " << p_.size() << endl;

    // for (unsigned i = 0; i < p.p_[0].size(); i++) {
    //     cout << setw(2);
    //     cout << i << " ";
    // }
    // cout << endl;

    for (auto v : p_) {
        for (auto i : v) {
            // cout << setw(2);
            cout << i << " ";
        }
        cout << endl;
    }
    return o;
}
#endif


void print_solve() {
    for (auto &s : solutions) {
        auto r = s->rowID;
        auto &v = p_[r];
        auto id = find(v.begin(), v.begin() + 12, true) - v.begin();

        char c = sn[id];
        // char c = (id == 0 ? 'L' :
        //           id == 1 ? 'P' :
        //           id == 2 ? 'S' :
        //           id == 3 ? 'F' :
        //           id == 4 ? 'H' :
        //           id == 5 ? 'Y' :
        //           id == 6 ? 'N' :
        //           id == 7 ? 'A' :
        //           id == 8 ? 'V' :
        //           id == 9 ? 'U' :
        //           id == 10 ? 'T' :
        //           id == 11 ? 'W' : '?');
        
        
        for (auto i = 12; i < nCol; i++) {
            if (v[i] == true)
                cell[i - 12] = c;
        }
    }

    for (auto i = 0; i < ROW; i++) {
        for (auto j = 0; j < COL; j++)
            cout << cell[i * COL + j] << " ";
        cout << endl;
    }
}

class Shape {
// protected:
public:
    Position position;
    vector<Position> piece;
    vector < vector<Position> > pieces;
    
    Shape(int x_, int y_);

    static Position rotate(Position c, Position p);
    static Position symmetry(Position c, Position p);
    vector<Position> rotatePiece(const vector<Position> &v);
    vector<Position> symmetryPiece(const vector<Position> &v);

    void saveRotatePieces(int);
    void saveSymmetryPieces(int);

    friend ostream & operator << (ostream &o, const Shape &s);
    friend class Possible;
};



vector<Position> Shape::rotatePiece(const vector<Position> &v) {
    vector<Position> v_;
    for (auto i : v) {
        v_.push_back(rotate(position, i));
    }
    return v_;
}

vector<Position> Shape::symmetryPiece(const vector<Position> &v) {
    vector<Position> v_;
    for (auto i : v) {
        v_.push_back(symmetry(position, i));
    }
    return v_;
}


void Shape::saveRotatePieces(int t) {
    pieces.push_back(piece);

    for (auto i = 0; i < t; i++) {
        pieces.push_back(rotatePiece(pieces.back()));
    }
}

void Shape::saveSymmetryPieces(int t) {

    pieces.push_back(symmetryPiece(piece));

    for (auto i = 0; i < t; i++) {
        pieces.push_back(rotatePiece(pieces.back()));
    }
}


inline ostream & operator << (ostream &o, const Shape &s) {
    for (auto i : s.pieces) {
        for (auto v : i) {
            o << "[" << v << "] ";
        }
        cout << endl;
    }
    return o;
}

class L_shape : public Shape {
public:
    L_shape(int x_, int y_);
};

class P_shape : public Shape {
public:
    P_shape(int x_, int y_);
};

class S_shape : public Shape {
public:
    S_shape(int x_, int y_);
};

class F_shape : public Shape {
public:
    F_shape(int x_, int y_);
};

class H_shape : public Shape {
public:
    H_shape(int x_, int y_);
};

class Y_shape : public Shape {
public:
    Y_shape(int x_, int y_);
};

class N_shape : public Shape {
public:
    N_shape(int x_, int y_);
};

class A_shape : public Shape {
public:
    A_shape(int x_, int y_);
};

class V_shape : public Shape {
public:
    V_shape(int x_, int y_);
};

class U_shape : public Shape {
public:
    U_shape(int x_, int y_);
};

class T_shape : public Shape {
public:
    T_shape(int x_, int y_);
};

class W_shape : public Shape {
public:
    W_shape(int x_, int y_);
};

bool isSafe(const vector<Position> &p) {
    for (auto i : p) {
        if (i.x < 0 || i.x >= ROW ||
            i.y < 0 || i.y >= COL)
            return false;
    }
    return true;
}

Position Shape::rotate(Position c, Position p) {
    // Rotate point p anticlockwise with c as the center.
    int x, y, dx, dy;

    dx = p.x - c.x;
    dy = p.y - c.y;

    x = -dy, y = dx;
    x += c.x, y += c.y;

    return Position(x, y);
}

Position Shape::symmetry(Position c, Position p) {
    int x, y, dx, dy;

    dx = p.x - c.x;
    dy = p.y - c.y;

    x = -dx, y = dy;
    x += c.x, y += c.y;

    return Position(x, y);
}

Shape::Shape(int x_, int y_) {
    position.x = x_, position.y = y_;
    // pieces = vector< vector<Position> >(4);
}


L_shape::L_shape(int x_, int y_) : Shape(x_, y_) {
    /*
       o
       o o o
    */


    piece.push_back(position);
    piece.push_back(Position(x_ + 1, y_));
    piece.push_back(Position(x_ + 1, y_ + 1));
    piece.push_back(Position(x_ + 1, y_ + 2));

    saveRotatePieces(3);
    saveSymmetryPieces(3);

}


P_shape::P_shape(int x_, int y_) : Shape(x_, y_) {
    /*
      o
      o o o o
    */


    piece.push_back(position);
    piece.push_back(Position(x_ + 1, y_));
    piece.push_back(Position(x_ + 1, y_ + 1));
    piece.push_back(Position(x_ + 1, y_ + 2));
    piece.push_back(Position(x_ + 1, y_ + 3));

    saveRotatePieces(3);
    saveSymmetryPieces(3);

}

S_shape::S_shape(int x_, int y_) : Shape(x_, y_) {
    /*
      o
      o o
        o
    */


    piece.push_back(position);
    piece.push_back(Position(x_ + 1, y_));
    piece.push_back(Position(x_ + 1, y_ + 1));
    piece.push_back(Position(x_ + 2, y_ + 1));

    saveRotatePieces(1);
    saveSymmetryPieces(1);

}

F_shape::F_shape(int x_, int y_) : Shape(x_, y_) {
    /*
      o
      o o
      o
      o
    */


    piece.push_back(position);
    piece.push_back(Position(x_ + 1, y_));
    piece.push_back(Position(x_ + 1, y_ + 1));
    piece.push_back(Position(x_ + 2, y_));
    piece.push_back(Position(x_ + 3, y_));

    saveRotatePieces(3);
    saveSymmetryPieces(3);

}

H_shape::H_shape(int x_, int y_) : Shape(x_, y_) {
    /*
      o
      o o
      o o
    */


    piece.push_back(position);
    piece.push_back(Position(x_ + 1, y_));
    piece.push_back(Position(x_ + 1, y_ + 1));
    piece.push_back(Position(x_ + 2, y_));
    piece.push_back(Position(x_ + 2, y_ + 1));

    saveRotatePieces(3);
    saveSymmetryPieces(3);

}

Y_shape::Y_shape(int x_, int y_) : Shape(x_, y_) {
    /*
      o
      o o o
        o
    */


    piece.push_back(position);
    piece.push_back(Position(x_ + 1, y_));
    piece.push_back(Position(x_ + 1, y_ + 1));
    piece.push_back(Position(x_ + 1, y_ + 2));
    piece.push_back(Position(x_ + 2, y_ + 1));

    saveRotatePieces(3);
    saveSymmetryPieces(3);
}

N_shape::N_shape(int x_, int y_) : Shape(x_, y_) {
    /*
      o
      o o
        o
        o
    */


    piece.push_back(position);
    piece.push_back(Position(x_ + 1, y_));
    piece.push_back(Position(x_ + 1, y_ + 1));
    piece.push_back(Position(x_ + 2, y_ + 1));
    piece.push_back(Position(x_ + 3, y_ + 1));

    saveRotatePieces(3);
    saveSymmetryPieces(3);
}

A_shape::A_shape(int x_, int y_) : Shape(x_, y_) {
    /*
      o
      o o
    */


    piece.push_back(position);
    piece.push_back(Position(x_ + 1, y_));
    piece.push_back(Position(x_ + 1, y_ + 1));

    saveRotatePieces(3);
}

V_shape::V_shape(int x_, int y_) : Shape(x_, y_) {
    /*
      o
      o
      o o o
    */


    piece.push_back(position);
    piece.push_back(Position(x_ + 1, y_));
    piece.push_back(Position(x_ + 2, y_));
    piece.push_back(Position(x_ + 2, y_ + 1));
    piece.push_back(Position(x_ + 2, y_ + 2));

    saveRotatePieces(3);
}

U_shape::U_shape(int x_, int y_) : Shape(x_, y_) {
    /*
      o   o
      o o o
    */


    piece.push_back(position);
    piece.push_back(Position(x_, y_ + 2));
    piece.push_back(Position(x_ + 1, y_));
    piece.push_back(Position(x_ + 1, y_ + 1));
    piece.push_back(Position(x_ + 1, y_ + 2));

    saveRotatePieces(3);
}

T_shape::T_shape(int x_, int y_) : Shape(x_, y_) {
    /*
      o o o
        o
    */


    piece.push_back(position);
    piece.push_back(Position(x_, y_ + 1));
    piece.push_back(Position(x_, y_ + 2));
    piece.push_back(Position(x_ + 1, y_ + 1));

    saveRotatePieces(3);
}

W_shape::W_shape(int x_, int y_) : Shape(x_, y_) {
    /*
      o o
        o o
          o
    */


    piece.push_back(position);
    piece.push_back(Position(x_, y_ + 1));
    piece.push_back(Position(x_ + 1, y_ + 1));
    piece.push_back(Position(x_ + 1, y_ + 2));
    piece.push_back(Position(x_ + 2, y_ + 2));

    saveRotatePieces(3);
}

// L P S F H Y N A V U T W   0, ..., COL*ROW
// 0                    11  12, ..., COL*ROW+12
void init() {
    header = new Node();
    vector<bool> _b(COL * ROW + 12, true);
    p_.push_back(_b);
    
    sn[0] = 'L';
    sn[1] = 'P';
    sn[2] = 'S';
    sn[3] = 'F';
    sn[4] = 'H';
    sn[5] = 'Y';
    sn[6] = 'N';
    sn[7] = 'A';
    sn[8] = 'V';
    sn[9] = 'U';
    sn[10] = 'T';
    sn[11] = 'W';

    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            vector<Shape *> shapeVec;
            vector<Shape *>::iterator shape;

            shapeVec.push_back(new L_shape(i, j));
            shapeVec.push_back(new P_shape(i, j));
            shapeVec.push_back(new S_shape(i, j));
            shapeVec.push_back(new F_shape(i, j));
            shapeVec.push_back(new H_shape(i, j));
            shapeVec.push_back(new Y_shape(i, j));
            shapeVec.push_back(new N_shape(i, j));
            shapeVec.push_back(new A_shape(i, j));
            shapeVec.push_back(new V_shape(i, j));
            shapeVec.push_back(new U_shape(i, j));
            shapeVec.push_back(new T_shape(i, j));
            shapeVec.push_back(new W_shape(i, j));

            for (shape = shapeVec.begin(); shape != shapeVec.end(); ++shape) {
                for (auto k : (*shape)->pieces) {
                    if (isSafe(k)) {
                        vector<bool> b(COL * ROW + 12, false);
                        b[shape - shapeVec.begin()] = true;
                        
                        for (auto v : k) {
                            b[12 + v.x * COL + v.y] = true;
                        }
                        p_.push_back(b);
                    }
                }
            }

            for (shape = shapeVec.begin(); shape != shapeVec.end(); ++shape)
                delete(*shape);
        }
    }

    nRow = p_.size();
    nCol = COL * ROW + 12;

    createNodeMatrix();

}



Node *createNodeMatrix() {
    Matrix = vector< vector<Node> >(p_.size());
    for (auto &v : Matrix)
        v = vector<Node>(COL * ROW + 12);

    // nRow = p_.size();
    // nCol = p_[0].size();

    // One extra row for list header nodes for each column.
    for (auto i = 0; i < nRow; i++) {
        for (auto j = 0; j < nCol; j++) {
            if (p_[i][j]) {
                int a, b;

                if (i)
                    Matrix[0][j].nodeCount += 1;
                
                // Add pointer to column header for this column node.
                Matrix[i][j].column = &Matrix[0][j];

                // set row and column id of this node
                Matrix[i][j].rowID = i;
                Matrix[i][j].colID = j;

                // Link the node with neighbors

                // Left pointer
                a = i, b = j;
                do {
                    b = getLeft(b);
                } while (!p_[a][b] && b != j);
                Matrix[i][j].left = &Matrix[i][b];

                // Right pointer
                a = i, b = j;
                do {
                    b = getRight(b);
                } while (!p_[a][b] && b != j);
                Matrix[i][j].right = &Matrix[i][b];

                // Up pointer
                a = i, b = j;
                do {
                    a = getUp(a);
                } while (!p_[a][b] && a != i);
                Matrix[i][j].up = &Matrix[a][j];

                // Down pointer
                a = i, b = j;
                do {
                    a = getDown(a);
                } while (!p_[a][b] && a != i);
                Matrix[i][j].down = &Matrix[a][j];
            }
        }
    }

    // Link header right pointer to column header of first column
    header->right = &Matrix[0][0];

    // Link header left pointer to column header of last column
    header->left = &Matrix[0][nCol-1];

    Matrix[0][0].left = header;
    Matrix[0][nCol-1].right = header;
    return header;
}

// Cover the given node completely
void cover(Node *targetNode) {
    // Node *row, *rightNode;

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

            Matrix[0][rightNode->colID].nodeCount -= 1;
        }
    }
}

// Uncover the given node completely
void uncover(Node *targetNode) {
    Node *colNode = targetNode->column;

    for (Node *row = colNode->up; row != colNode; row = row->up) {
        for (Node *leftNode = row->left; leftNode != row;
             leftNode = leftNode->left) {
            leftNode->up->down = leftNode;
            leftNode->down->up = leftNode;

            Matrix[0][leftNode->colID].nodeCount += 1;
        }
    }

    // Link the column header from it's neighbors
    colNode->left->right = colNode;
    colNode->right->left = colNode;
}

void solve(void) {

    if (header->right == header) {
        // if (solutions.size() == 12) {
            
        // cout << solutions.size() << endl;
        print_solve();
        cell.clear();
        cout << endl;
        if (++global == runs)
            exit(0);
        // }

        return;
    }

    Node *column = least_one();
    if (column == header)
        return;

    cover(column);
    for (Node *row = column->down; row != column; row = row->down) {
        solutions.push_back(row);

        for (Node *rightNode = row->right; rightNode != row;
             rightNode = rightNode->right)
            cover(rightNode);

        solve();

        solutions.pop_back();
        column = row->column;
        for (Node *leftNode = row->left; leftNode != row;
             leftNode = leftNode->left)
            uncover(leftNode);
    }
    uncover(column);
}


int main(int argc, char *argv[]) {
    runs = atoi(argv[1]);
    
    // Possible possible;
    init();
    
    // cout << possible << endl;
    solve();
}


