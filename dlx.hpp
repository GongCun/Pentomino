#ifndef _MY_DLX_H
#define _MY_DLX_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <stdlib.h>
#include <cstdio>
#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/pointer.h"
// #include "rapidjson/filereadstream.h"

#define BUFLEN 4096
// #define MAXBUF 65536
using namespace std;
using namespace rapidjson;

extern int runs;
extern string puzzle;
extern void writeString(string&);
extern void print_solve(ostream& o, vector<int>&);

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
    
public:
    Node *header;
    int nCol, nRow;
    vector< vector<Node> > matrix;
    vector<int> solutions;

    DLX(vector< vector<bool> >& p_);
    DLX(Node*&, int&, int&, vector<int>&);
    DLX(istream&, string&);

    ~DLX() {
        if (!header) delete header;
        header = NULL;
        for (auto &v: matrix)
            v.clear();
        matrix.clear();
    }

    // void free() {
    //     this->DLX::~DLX();
    // }
    
    Node *leastOne(void);
    void cover(Node *);
    void uncover(Node *);
    bool solve(ostream &);
    void print();

    int getRight(int i) { return (i + 1) % nCol; }
    int getLeft(int i) { return (i - 1 < 0) ? nCol - 1 : i - 1; }
    int getUp(int i) { return (i - 1 < 0) ? nRow - 1 : i - 1; }
    int getDown(int i) { return (i + 1) % nRow; }

};

void distribute(unsigned, DLX*);
void dlxSerialize(string&, DLX*);

enum State { idle = 0, in_progress, completed };
typedef struct sockaddr_in SA;
struct taskinfo {
    pid_t   pid;
    int     fd;
    SA      sa;
    char   *ip;
    string  input;
    State   state;
};
extern vector<taskinfo>tasklist;
#endif
