#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

const int ROW = 5;
const int COL = 11;
// const int ROW = 2;
// const int COL = 3;

class Position {
public:
    int x, y;
    Position() : x(0), y(0) {}
    
    Position(int x_, int y_) :
        x(x_), y(y_) {}

    // friend class Shape;
    friend ostream & operator << (ostream &o, const Position &p);
};

inline ostream & operator << (ostream &o, const Position &p) {
    o << p.x << ", " << p.y;
    return o;
}

class Possible {
    vector< vector<int> > p_;
public:
    Possible(void);

    bool least_one(vector<unsigned> &vRow);
    void x(unsigned r);

    friend ostream & operator << (ostream &o, const Possible &p);
};

bool Possible::least_one(vector<unsigned> &vRow) {
    int min = -1, k, c = -1;
    for (unsigned j = 1; j < p_[0].size(); j++) {
        k = 0;
        for (unsigned i = 0; i < p_.size(); i++) {
            k += p_[i][j];
        }
        if (min == -1 || k < min) {
            min = k;
            c = j;
        }
    }
    if (min == 0) return false;

    for (unsigned i = 0; i < p_.size(); i++) {
        if (p_[i][c])
            vRow.push_back(i);
    }
    return true;
}

#if 0
inline ostream & operator << (ostream &o, const Possible &p) {
    cout << "total: " << p.p_.size() << endl;

    // for (unsigned i = 0; i < p.p_[0].size(); i++) {
    //     cout << setw(2);
    //     cout << i << " ";
    // }
    // cout << endl;

    for (auto v : p.p_) {
        for (auto i : v) {
            // cout << setw(2);
            cout << i << " ";
        }
        cout << endl;
    }
    return o;
}
#endif


inline ostream & operator << (ostream &o, const Possible &p) {
    cout << "total: " << p.p_.size() << endl;
    
    for (auto v : p.p_) {
        cout << "idx = " << v[0] << " ";
        auto s = find(v.begin() + 1, v.begin() + 13, true);

        switch (s - v.begin() - 1) {
        case 0:
            cout << "L: ";
            break;
        case 1:
            cout << "P: ";
            break;
        case 2:
            cout << "S: ";
            break;
        case 3:
            cout << "F: ";
            break;

        case 4:
            cout << "H: ";
            break;
        case 5:
            cout << "Y: ";
            break;
        case 6:
            cout << "N: ";
            break;
        case 7:
            cout << "A: ";
            break;
        case 8:
            cout << "V: ";
            break;
        case 9:
            cout << "U: ";
            break;
        case 10:
            cout << "T: ";
            break;
        case 11:
            cout << "W: ";
            break;
        default:
            cout << "Unknown: ";
        }

        // cout << v.size() << endl;
        for (unsigned i = 13; i < v.size(); i++) {
            if (v[i] == true)
                // cout << "<" << (i-13) / COL << ", " << (i-13) % COL << "> ";
                cout << (i-13) / COL << "," << (i-13) % COL << " ";
        }
        cout << endl;
    }
    return o;
}

class Shape {
protected:
    Position position;
    vector<Position> piece;
    vector < vector<Position> > pieces;
    
public:
    Shape(int x_, int y_);

    static Position rotate(Position c, Position p);
    static Position symmetry(Position c, Position p);
    vector<Position> rotatePiece(const vector<Position> &v);
    vector<Position> symmetryPiece(const vector<Position> &v);

    void saveRotatePieces(void);
    void saveRotatePieces(int);
    void saveSymmetryPieces(void);
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

void Shape::saveRotatePieces() {
    pieces.push_back(piece);

    for (auto i = 0; i < 3; i++) {
        pieces.push_back(rotatePiece(pieces.back()));
    }
}

void Shape::saveRotatePieces(int t) {
    pieces.push_back(piece);

    for (auto i = 0; i < t; i++) {
        pieces.push_back(rotatePiece(pieces.back()));
    }
}

void Shape::saveSymmetryPieces(void) {

    pieces.push_back(symmetryPiece(piece));

    for (auto i = 0; i < 3; i++) {
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

    saveRotatePieces();
    saveSymmetryPieces();

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

    saveRotatePieces();
    saveSymmetryPieces();

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

    saveRotatePieces();
    saveSymmetryPieces();

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

    saveRotatePieces();
    saveSymmetryPieces();

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

    saveRotatePieces();
    saveSymmetryPieces();
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

    saveRotatePieces();
    // saveSymmetryPieces();
}

A_shape::A_shape(int x_, int y_) : Shape(x_, y_) {
    /*
      o
      o o
    */


    piece.push_back(position);
    piece.push_back(Position(x_ + 1, y_));
    piece.push_back(Position(x_ + 1, y_ + 1));

    saveRotatePieces(2);
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

    saveRotatePieces(2);
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

    saveRotatePieces();
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

    saveRotatePieces();
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

    saveRotatePieces();
}

// idx L P S F H Y N A V U T W   0, ..., 54
// 0   1                    12  13, ..., 67
Possible::Possible() {
    int idx = 0;
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
                        vector<int> b(68, false);
                        b[0] = idx++;
                        // cout << "<" << b[0] << ">" << endl;
                        b[1 + shape - shapeVec.begin()] = true;
                        for (auto v : k) {
                            b[13 + v.x * COL + v.y] = true;
                        }
                        p_.push_back(b);
                    }
                }
            }

            for (shape = shapeVec.begin(); shape != shapeVec.end(); ++shape)
                delete(*shape);
        }
    }
}

void Possible::x(unsigned r) {
    vector<unsigned> dr, dc;

    if (p_.empty())
        return;

    for (unsigned j = 1; j < p_[r].size(); j++) {
        if (p_[r][j]) {
            dc.push_back(j);
            for (unsigned i = 0; i < p_.size(); i++) {
                if (p_[i][j] == true &&
                    find(dr.begin(), dr.end(), i) == dr.end()) {
                    // cout << "(" << i << ", " << j << ") ";
                    dr.push_back(i);
                }
            }
            // cout << endl;
        }
    }

    sort(dr.begin(), dr.end());

    // for (auto v : dc) {
    //     cout << "c = " << v << " ";
    // }
    // cout  << endl;

    // for (auto v : dr) {
    //     cout << "r = " << v << " ";
    // }
    // cout << endl;

    unsigned i = 0;
    auto k = dr.begin();

    // delete row from matrix
    for (auto it = p_.begin(); it != p_.end() && k != dr.end(); ) {
        if (i++ == *k) {
            it = p_.erase(it);
            k++;
        } else {
            it++;
        }
    }

    // delete column from matrix
    for (auto &b : p_) {
        i = 0;
        k = dc.begin();
        for (auto it = b.begin(); it != b.end() && k != dc.end(); ) {
            if (i++ == *k) {
                it = b.erase(it);
                k++;
            } else {
                it++;
            }
        }
    }
}

int main(void) {
    Possible possible;
    cout << possible << endl;
    // possible.x(0);
    // cout << possible << endl;
    // possible.x(0);
    // cout << possible << endl;
}


