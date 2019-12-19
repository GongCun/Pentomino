#include "dlx.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>

#define MAXLINE 4096

extern vector<char *>serverList;

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
