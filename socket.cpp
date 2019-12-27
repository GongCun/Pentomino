#include "dlx.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>

#define MAXLINE 4096

extern vector<char *>serverList;
vector<int>sockfd;
fd_set allset;

void initSock() {
    FD_ZERO(&allset);
}

void writeSocket(char *port, string& str) {
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

    sockfd.push_back(sock);
    FD_SET(sock, &allset);
    // if (sock > maxfd) maxfd = sock;

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

    // if (close(sock) < 0) {
    //     perror("close");
    //     exit(-1);
    // }
    if (shutdown(sock, SHUT_WR) < 0) {
        perror("shutdown");
        exit(-1);
    }
}

void waitSlave() {
    int n, maxfd;
    fd_set rset;

    for ( ; ; ) {
        rset = allset;
        maxfd = *max_element(sockfd.begin(), sockfd.end());
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
                FD_CLR(fd, &allset);
                fprintf(stderr, "%d closed\n", fd);
                it = sockfd.erase(it);
            } else {
                ++it;
            }
        }

        if (sockfd.empty()) break;
    }
}
