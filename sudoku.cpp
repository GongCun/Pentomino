#include "dlx.hpp"
#include <map>

int runs;
char *input;
int branch;
bool master = false;
string puzzle;
char *port;
char *output;

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
    fprintf(stderr, "%s -m -i input -o output -b branches -r runs -s server -p port <json\n", s);
    exit(-1);
}

extern void writeSocket(char *, string&);
extern void waitSlave(void);
extern void initSock(void);

void writeString(string& str) {
    writeSocket(port, str);
}

int main(int argc, char *argv[]) {
    int c;

    while ((c = getopt(argc, argv, "mi:b:r:s:p:o:")) != EOF) {
        switch (c) {
        case 'm' :
            master = true;
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
        case 'o':
            output = optarg;
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
        initSock();
        distribute(branch, new DLX(possible));
        waitSlave();
        return 0;
    }

    // Slave process
    DLX dlx(cin, puzzle);
    init(puzzle);
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

