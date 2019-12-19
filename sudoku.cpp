#include "dlx.hpp"
#include <map>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>

#define MAXLINE 4096


int runs;
char *input;
int branch;
int master;
string puzzle;
// char *server;
char *port;

vector < vector<bool> > possible;
vector < vector<int> > _groups_of(81);
vector<char *>serverList;

const int nCol = 9 * (9 + 9 + 9 + 9);

void assign(vector< vector<bool> > &p, int k, int val) {
    vector<bool> v(nCol, false);
    v[k] = true;
    for (auto i : _groups_of[k])
        v[i * 9 + val + 81] = true;
    p.push_back(v);
}

void assign(vector< vector<bool> > &p, int k) {
    for (int val = 0; val < 9; val++) {
        vector<bool> v(nCol, false);
        v[k] = true;
        for (auto i : _groups_of[k]) {
            v[i * 9 + val + 81] = true;
        }
        p.push_back(v);
    }
}

void init(string s) {
    int k = 0;

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            const int k = i*9 + j;
            const int x[3] = {i, 9 + j, 18 + (i/3)*3 + j/3};
            for (int g = 0; g < 3; g++) {
                _groups_of[k].push_back(x[g]);
            }
        }
    }

    vector<bool> v(nCol, true);
    possible.push_back(v);

    for (unsigned i = 0; i < s.size(); i++) {
        if (s[i] >= '1' && s[i] <= '9') {
            assign(possible, k++, s[i] - '1');
        } else if (s[i] == '0' || s[i] == '.') {
            assign(possible, k++);
        }
    }
}

void print_solve(ostream &o, vector<int>& solutions) {
    map<int, int> grid;
    const string sep(6, '-');

    for (auto &v : solutions) {
        // auto r = possible[v->rowID];
        auto r = possible[v];
        int id = find(r.begin(), r.begin() + 81, true) - r.begin();
        int val = (find(r.begin() + 81, r.begin() + 81 * 2, true) - (r.begin() + 81)) % 9;
        grid[id] = val;
    }

    for (int i = 0; i < 9; i++) {
        if (i == 3 || i == 6)
            o << sep << "+-" << sep << "+-" << sep << endl;
        for (int j = 0; j < 9; j++) {
            if (j == 3 || j == 6)
                o << "| ";
            o << grid[i * 9 + j] + 1 << " ";
        }
        o << endl;
    }

    o << endl;
}

static void help(const char *s) {
    fprintf(stderr, "%s -m -i puzzle.txt -b branches -r runs -s server -p port <json\n", s);
    exit(-1);
}


static void writeSocket(char *port, string& str) {
    int sock = -1;
    struct sockaddr_in serv;
    static vector<char *>::iterator it = serverList.begin();

    if (it == serverList.end())
        it = serverList.begin();

    char *server = *it++;
    fprintf(stderr, "server: %s\n", server);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(-1);
    }

    bzero(&serv, sizeof(struct sockaddr_in));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(atoi(port));

    if (inet_pton(AF_INET, server, &serv.sin_addr) <= 0) {
        perror("invalid address");
        exit(-1);
    }

    if (connect(sock, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("connect");
        exit(-1);
    }

    const char *p = str.c_str();
    int len = str.size();
    while (len > 0) {
        size_t writelen = (MAXLINE < len ? MAXLINE : len);
        if (write(sock, p, writelen) < 0) {
            perror("write");
            exit(-1);
        }
        p += writelen;
        len -= writelen;
    }

    if (close(sock) < 0) {
        perror("close");
        exit(-1);
    }
}

void writeString(string& str) {
    writeSocket(port, str);
}

int main(int argc, char *argv[]) {
    int c;
    

    while ((c = getopt(argc, argv, "mi:b:r:s:p:")) != EOF) {
        switch (c) {
        case 'm' :
            master = 1;
            break;
        case 'i':
            input = optarg;
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
    // if (argc > 1) k = atoi(argv[1]);
    // if (argc > 2) runs = atoi(argv[2]);

    if (master) {
        if (!input) help(argv[0]);
        if (serverList.empty() || !port) help(argv[0]);

        filebuf fb;
        fb.open(input, ios::in);
        istream is(&fb);
        getline(is, puzzle);
        init(puzzle);
        distribute(branch, new DLX(possible));
        return 0;
    }

    DLX dlx(cin, puzzle);
    init(puzzle);
    if (!dlx.solve())
        cout << "No Solutions" << endl;

}

