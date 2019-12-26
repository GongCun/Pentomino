// #include <iostream>
// #include <vector>
// #include <algorithm>
// #include <map>
// #include <memory>
// #include <unistd.h>
// #include <stdlib.h>
#include "dlx.hpp"
#include <map>
using namespace std;

const int ROW = 5;
const int COL = 11;

int nRow;
int nCol;
int runs;
int branch;
bool master = false;
string puzzle;
char *port;
vector<char *>serverList;
vector<int>sockfd;
fd_set rset;
int maxfd;

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


void print_solve(ostream &o, vector<int>& solutions) {
    for (auto &r : solutions) {
        // auto r = s->rowID;
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
            o << cell[i * COL + j] << " ";
        o << endl;
    }
    o << endl;
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

}


extern void writeSocket(char *, string&);

void writeString(string& str) {
    writeSocket(port, str);
}

static void help(const char *s) {
    fprintf(stderr, "%s -m -b branches -r runs -s server -p port <json\n", s);
    exit(-1);
}

static void waitSlave() {
    int n;
    for ( ; ; ) {
        if ((n = select(maxfd + 1, &rset, NULL, NULL, NULL)) < 0) {
            perror("select");
            exit(-1);
        } else if (n == 0) {
            break;
        }

        for (vector<int>::iterator it = sockfd.begin();
             it != sockfd.end(); ) {
            int fd = *it;
            if (FD_ISSET(fd, &rset)) {
                if (close(fd) < 0) {
                    perror("close");
                    exit(-1);
                }
                FD_CLR(fd, &rset);
                fprintf(stderr, "%d closed\n", fd);
                it = sockfd.erase(it);
            } else {
                ++it;
            }
        }

        if (sockfd.empty()) break;
    }
}

int main(int argc, char *argv[]) {
    int c;

    while ((c = getopt(argc, argv, "mb:r:s:p:")) != EOF) {
        switch (c) {
        case 'm' :
            master = 1;
            break;
        case 'b':
            branch = atoi(optarg);
            break;
        case 'r':
            runs = atoi(optarg);
            break;
        case 's':
            // server = optarg;
            serverList.push_back(optarg);
            break;
        case 'p':
            port = optarg;
            break;
        case '?':
            help(argv[0]);
        }
    }
    init();
    
    if (master) {
        if (serverList.empty() || !port) help(argv[0]);

        FD_ZERO(&rset);
        distribute(branch, new DLX(p_));
        waitSlave();
        return 0;
    }

    DLX dlx(cin, puzzle);
    char buf[BUFLEN];
    snprintf(buf, sizeof(buf), "/tmp/solution.%u", (unsigned)getpid());
    ofstream ofs;
    ofs.open(buf, ofstream::out | ofstream::trunc);
    if (!ofs.is_open()) {
        fprintf(stderr, "open %s error: %s\n", buf, strerror(errno));
        exit(-1);
    }
    fprintf(stderr, "%s\n", buf);
    if (!dlx.solve(ofs))
        cout << "No Solutions" << endl;
    ofs.close();

}


