// #include <iostream>
// #include <vector>
// #include <algorithm>
// #include <map>
// #include <memory>
// #include <unistd.h>
// #include <stdlib.h>
#include "dlx.hpp"
#include <map>
// #define _GNU_SOURCE
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
char *output;
vector<char *>serverList;
char *exclude;
time_t start;
vector<taskinfo>tasklist;
char *input;
int tasks;
int offset, timeout;
int cmd_argc;
char *cmd[4096];


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
// extern void waitSlave(void);
// extern void initSock(void);

static bool task_completed(void) {
    for (auto &t : tasklist)
        if (t.state == in_progress) return false;

    return true; // idle or completed
}

void waitSlave() {
    pid_t pid;

    for ( ; ; ) {
        while ((pid = waitpid((pid_t)-1, NULL, 0)) > 0) {
            for (auto it = tasklist.begin(); it != tasklist.end(); ++it) {
                if (it->pid == pid) {
                    it->state = completed;
                    close(it->fd);
                    time_t t = time(NULL);
                    it->end = t;
                    fprintf(stderr, "pcocess %ld completed at %ld sec, fd %d, ip %s, data %s\n",
                            (long)it->pid, t - start, it->fd, it->ip, (it->input).c_str());
                    break;
                }
            }
        }
        if (errno != EINTR || task_completed())
            break;
    }

}

void writeString(string& str) {
    /*
    sigset_t newmask, oldmask;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGALRM);
    sigaddset(&newmask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        perror("sigprocmask");
        exit(-1);
    }*/

    // critical region
    writeSocket(port, str);

    // restore signal mask, will unblock SIGALRM/SIGCHLD
    /*
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        perror("sigprocmask");
        exit(-1);
    }*/

}

static void help(const char *s) {
    fprintf(stderr, "%s -m -b branches -r runs -s server -p port -o output -i input -t timeout -z offset -x exclude <json\n", s);
    exit(-1);
}

static void sig_alarm(int signo) {
    int runs = 0;
    for (auto &v: tasklist) {
        if (v.state == in_progress)
            ++runs;
    }
    fprintf(stderr, "escape %ld sec, in-progress tasks = %d\n", time(NULL) - start, runs);

    for (vector<taskinfo>::iterator ptr = tasklist.begin();
         ptr != tasklist.end(); ++ptr) {

        sigset_t newmask, oldmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGCHLD);
        sigaddset(&newmask, SIGALRM);
        if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
            perror("sigprocmask");
            exit(-1);
        }

        auto v = *ptr;
        if (v.state == in_progress && 
                time(NULL) - v.start > timeout &&
                v.backup == false) {


            if (kill(v.pid, SIGTERM) < 0) {
                perror("kill");
                exit(-1);
            }

            pid_t pid;
            time_t t = time(NULL);
            if ((pid = fork()) == 0) {
                cmd[cmd_argc] = strdup("-i");
                cmd[cmd_argc + 1] = strdup(v.input.c_str());
                cmd[cmd_argc + 2] = strdup("-z");
                if (asprintf(&cmd[cmd_argc + 3], "%ld", t - start) < 0) {
                    perror("asprintf");
                    exit(-1);
                }
                cmd[cmd_argc + 4] = strdup("-x");
                cmd[cmd_argc + 5] = strdup(v.ip);
                cmd[cmd_argc + 6] = NULL;

                sigset_t mask;
                sigemptyset(&mask);
                sigaddset(&mask, SIGALRM);
                sigaddset(&mask, SIGCHLD);
                if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) {
                    perror("sigprocmask");
                    exit(-1);
                }

                execv(cmd[0], cmd);
                exit(-1);
            } else if (pid < 0) {
                perror("fork");
                exit(-1);
            }

            fprintf(stderr, "backup %ld start at %ld sec, tasks: %d, data: %s\n",
                    (long)pid, t - start, ++tasks, v.input.c_str());

            struct taskinfo taskinfo;
            //bzero(&taskinfo, sizeof(taskinfo));
            taskinfo.start = t;
            taskinfo.end = 0;
            taskinfo.ip = NULL;
            taskinfo.fd = -1;
            taskinfo.pid = pid;
            taskinfo.state = in_progress;
            taskinfo.input = v.input;
            taskinfo.backup = true; // don't re-execute the backup tasks
            tasklist.push_back(taskinfo);
        }

        if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
            perror("sigprocmask");
            exit(-1);
        }
    }

    alarm(1);
}

static void sig_chld(int signo) {
    pid_t pid;
    int status;

    while ((pid = waitpid((pid_t)-1, &status, WNOHANG)) > 0) {
        for (auto it = tasklist.begin(); it != tasklist.end(); ++it) {
            if (it->pid == pid) {
                it->state = completed;
                close(it->fd);
                if (WIFSIGNALED(status))
                    fprintf(stderr, "signal %d ", WTERMSIG(status));
                fprintf(stderr, "pcocess %ld completed at %ld sec, fd %d, ip %s, data %s\n",
                        (long)it->pid, time(NULL) - start, it->fd, it->ip, (it->input).c_str());
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int c;
    char trace[1024];

    start = time(NULL);

    while ((c = getopt(argc, argv, "mb:r:s:p:o:i:t:z:x:")) != EOF) {
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
        case 'o':
            output = optarg;
            break;
        case 'i':
            input = optarg;
            break;
        case 't':
            timeout = atoi(optarg);
            break;
        case 'z':
            offset = atoi(optarg);
            start -= offset;
            break;
        case 'x':
            exclude = optarg;
            break;
        case '?':
            help(argv[0]);
        }
    }

    for (cmd_argc = 0; cmd_argc < argc; cmd_argc++)
        cmd[cmd_argc] = strdup(argv[cmd_argc]);

    init();
    
    if (master) {
        if (serverList.empty() || !port) help(argv[0]);

        sprintf(trace, "./trace.%ld", (long)getpid());
        if (freopen(trace, "w+", stderr) == NULL) {
            perror("freopen");
            exit(-1);
        }
        if (setvbuf(stderr, NULL, _IONBF, 0) != 0) {
            printf("setvbuf error\n");
            exit(-1);
        }
        printf("trace file: %s\n", trace);

        // initSock();
        if (signal(SIGALRM, sig_alarm) == SIG_ERR) {
            perror("signal");
            exit(-1);
        }
        if (signal(SIGCHLD, sig_chld) == SIG_ERR) {
            perror("signal");
            exit(-1);
        }
        alarm(1);

        if (!input) {
            distribute(branch, new DLX(p_));
        } else {
            ifstream myfile(input);
            if (!myfile.is_open()) {
                fprintf(stderr, "open %s: %s\n", input, strerror(errno));
                exit(-1);
            }
            distribute(branch, new DLX(myfile, puzzle));
            myfile.close();
        }

        // if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) {
        //     perror("signal");
        //     exit(-1);
        // }
        waitSlave();

        return 0;
    }

    // Slave process
    DLX dlx(cin, puzzle);
    if (output == NULL) {
        help(argv[0]);
    }
    ofstream ofs;
    ofs.open(output, ofstream::out | ofstream::trunc);
    if (!ofs.is_open()) {
        fprintf(stderr, "open %s error: %s\n", output, strerror(errno));
        exit(-1);
    }
    fprintf(stderr, "%s\n", output);
    if (!dlx.solve(ofs))
        ofs << "No Solutions" << endl;
    ofs.close();

}


