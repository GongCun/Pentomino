#include "dlx.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

#define MAXLINE 4096

extern vector<char *>serverList;
extern time_t start;
int tasks;

void writeSocket(char *port, string& str) {
    int sock = -1;
    pid_t pid;
    
    struct sockaddr_in serv;
    static vector<char *>::iterator it = serverList.begin();

    if (it == serverList.end())
        it = serverList.begin();

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

        if (shutdown(sock, SHUT_WR) < 0) {
            perror("shutdown");
            exit(-1);
        }

        // Waiting the task finish
        fd_set rset;
        FD_ZERO(&rset);
        FD_SET(sock, &rset);
        // suppose task process won't say anything
        if (select(sock + 1, &rset, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(-1);
        }
        exit(0);
    } // child process exited


    // Parent process continued: record the process information and input data
    // for re-execute the backup tasks.

    char tmpfile[] = "./inputXXXXXX";
    int f;
    if ((f = mkstemp(tmpfile)) < 0) {
        perror("mkstemp");
        exit(-1);
    }
    close(f);

    fprintf(stderr, "process %ld start at %ld sec, tasks: %d, ip: %s, fd: %d, data: %s\n",
            (long)pid, time(NULL) - start, ++tasks, server, sock, tmpfile);
    
    struct taskinfo taskinfo;
    taskinfo.pid = pid;
    taskinfo.fd = sock; // keep the fd open for backup tasks
    taskinfo.ip = server;
    taskinfo.input = string(tmpfile);
    taskinfo.sa = serv; // struct copy
    taskinfo.state = in_progress;
    tasklist.push_back(taskinfo);
    ofstream o(tmpfile);
    if (!o.is_open()) {
        fprintf(stderr, "open %s error: %s\n", tmpfile, strerror(errno));
        exit(-1);
    }
    o << str;
    o.close();

}

// static bool comp(sockinfo a, sockinfo b) {
//     return (a.fd < b.fd);
// }
