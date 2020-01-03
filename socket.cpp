#include "dlx.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>

#define MAXLINE 4096

extern vector<char *>serverList;
extern time_t start;
struct sockinfo {
    int fd;
    string address;
};
vector<sockinfo>socklist;
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

    fprintf(stderr, "open: %s, fd: %d\n", server, sock);

    struct sockinfo sockinfo = {
        .fd = sock,
        .address = server
    };
    
    socklist.push_back(sockinfo);
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

static bool comp(sockinfo a, sockinfo b) {
    return (a.fd < b.fd);
}

void waitSlave() {
    int n, maxfd;
    fd_set rset;

    for ( ; ; ) {
        rset = allset;
        maxfd = max_element(socklist.begin(), socklist.end(), comp)->fd;
        
        if ((n = select(maxfd + 1, &rset, NULL, NULL, NULL)) < 0) {
            perror("select");
            exit(-1);
        } else if (n == 0) {
            break;
        }

        for (vector<sockinfo>::iterator it = socklist.begin();
             it != socklist.end(); ) {
            int fd = it->fd;

            // statistic the task over time
            cout << "escape: " << time(NULL) - start
                 << " tasks: " << socklist.size() << endl;
            
            if (FD_ISSET(fd, &rset)) {
                cout << "close: "
                     << it->address
                     << ", fd: " << fd << endl;
                if (close(fd) < 0) {
                    perror("close");
                    exit(-1);
                }
                FD_CLR(fd, &allset);
                // fprintf(stderr, "%d closed\n", fd);
                it = socklist.erase(it);
            } else {
                ++it;
            }
        }

        if (socklist.empty()) break;
    }
}
