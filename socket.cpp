#include "dlx.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

#define MAXLINE 4096

extern vector<char *>serverList;
extern time_t start;
extern int tasks;
extern char *exclude;

static void writeStr(const char *str, int fd) {
    const char *p = str;
    int len = strlen(str);
    while (len > 0) {
        size_t writelen = (MAXLINE < len ? MAXLINE : len);
        if (write(fd, p, writelen) < 0) {
            perror("write");
            exit(-1);
        }
        p += writelen;
        len -= writelen;
    }
    if (write(fd, "\n", 1) != 1) {
        perror("write");
        exit(-1);
    }

}

void writeSocket(char *port, DLX *dlx) {
    int sock = -1;
    pid_t pid;
    
    struct sockaddr_in serv;
    static vector<char *>::iterator it = serverList.begin();

a:
    if (it == serverList.end())
        it = serverList.begin();

    if (exclude && strcmp(exclude, *it) == 0 && serverList.size() != 1) {
        it++;
        goto a;
    }

    char *server = *it++;

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


    char tmpfile[] = "./inputXXXXXX";
    int f;
    if ((f = mkstemp(tmpfile)) < 0) {
        perror("mkstemp");
        exit(-1);
    }
    // close(f);

    // Fork the task for doing the map work.
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(-1);
    }

    if (pid == 0) {
        
        if (connect(sock, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
            perror("connect");
            exit(-1);
        }

        // Pipeline the JSON stream
        Node *h = dlx->header;
        Document d;
        Pointer("/puzzle").Set(d, puzzle.c_str());
        Pointer("/nCol").Set(d, dlx->nCol);
        Pointer("/nRow").Set(d, dlx->nRow);
        for (unsigned i = 0;  i < dlx->solutions.size(); i++) {
            string s = "/solutions/" + to_string(i);
            Pointer(s.c_str()).Set(d, dlx->solutions[i]);
        }

        StringBuffer sb;
        Writer<StringBuffer> writer(sb);
        d.Accept(writer);
        writeStr(sb.GetString(), sock); // writeStr will end with '\n'
        writeStr(sb.GetString(), f); // for backup tasks

        for (Node *col = h->right; col != h; col = col->right) {
            Node *row = col;
            do {
                Document d_;
            
                Pointer("/matrix/nodeCount").Set(d_, row->nodeCount);
                Pointer("/matrix/rowID").Set(d_, row->rowID);
                Pointer("/matrix/colID").Set(d_, row->colID);
                Pointer("/matrix/left").Set(d_, row->left->colID);
                Pointer("/matrix/right").Set(d_, row->right->colID);
                Pointer("/matrix/up").Set(d_, row->up->rowID);
                Pointer("/matrix/down").Set(d_, row->down->rowID);

                StringBuffer sb_;
                Writer<StringBuffer> writer_(sb_);
                d_.Accept(writer_);
                writeStr(sb_.GetString(), sock); // writeStr end with '\n'
                writeStr(sb_.GetString(), f); // for backup tasks

                row = row->down;
            } while (row != col);
        }

        if (h->right != h) {
            Document d_;
            Pointer("/front").Set(d_, h->right->colID);
            Pointer("/back").Set(d_, h->left->colID);
            StringBuffer sb_;
            Writer<StringBuffer> writer_(sb_);
            d_.Accept(writer_);
            writeStr(sb_.GetString(), sock); // writeStr end with '\n'
            writeStr(sb_.GetString(), f); // for backup tasks
        }

        close(f);

        if (shutdown(sock, SHUT_WR) < 0) {
            perror("shutdown");
            exit(-1);
        }

        // Waiting the task finish
        fd_set saveset, rset;
        FD_ZERO(&saveset);
        FD_SET(sock, &saveset);
        int n;
        char buf[MAXLINE];

        for ( ; ; ) {
            rset = saveset;
            if (select(sock + 1, &rset, NULL, NULL, NULL) < 0) {
                perror("select");
                exit(-1);
            }
            if (FD_ISSET(sock, &rset)) {
                n = read(sock, buf, MAXLINE);
                if (n < 0) {
                    perror("read buf");
                    exit(-1);
                } else if (n == 0) {
                    break;
                }
            }
        }
        
        exit(0);
    } // child process exited


    // Parent process continued: record the process information and input data
    // for re-execute the backup tasks.
    close(f);

    time_t t = time(NULL);
    fprintf(stderr, "process %ld start at %ld sec, tasks: %d, ip: %s, fd: %d, data: %s\n",
            (long)pid, t - start, ++tasks, server, sock, tmpfile);

    struct taskinfo taskinfo;
    //bzero(&taskinfo, sizeof(struct taskinfo));
    taskinfo.backup = false;
    taskinfo.start = t;
    taskinfo.end = 0;
    taskinfo.pid = pid;
    taskinfo.fd = sock; // keep the fd open for backup tasks
    taskinfo.ip = server;
    taskinfo.input = string(tmpfile);
    taskinfo.sa = serv; // struct copy
    taskinfo.state = in_progress;
    tasklist.push_back(taskinfo);
    // ofstream o(tmpfile);
    // if (!o.is_open()) {
    //     fprintf(stderr, "open %s error: %s\n", tmpfile, strerror(errno));
    //     exit(-1);
    // }
    // o << str;
    // o.close();

}

void dlxSerialize(string &str, DLX *dlx) {
    Document d;
    int g = 0;

    Pointer("/puzzle").Set(d, puzzle.c_str());
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
            string id = "/matrix/" + to_string(g++);
            
            Pointer((id + "/nodeCount").c_str()).Set(d, row->nodeCount);
            Pointer((id + "/rowID").c_str()).Set(d, row->rowID);
            Pointer((id + "/colID").c_str()).Set(d, row->colID);
            Pointer((id + "/left").c_str()).Set(d, row->left->colID);
            Pointer((id + "/right").c_str()).Set(d, row->right->colID);
            Pointer((id + "/up").c_str()).Set(d, row->up->rowID);
            Pointer((id + "/down").c_str()).Set(d, row->down->rowID);

            row = row->down;
        } while (row != col);
    }

    if (h->right != h) {
        Pointer("/front").Set(d, h->right->colID);
        Pointer("/back").Set(d, h->left->colID);
    }

    StringBuffer sb;
    // PrettyWriter<StringBuffer> writer(sb);
    Writer<StringBuffer> writer(sb);
    d.Accept(writer);
    // puts(sb.GetString());
    // o << sb.GetString() << endl;
    str = string(sb.GetString());
}
