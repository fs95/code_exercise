#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "Common.h"

using namespace std;

// Initialize the server
int InitServer(int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
    // Record error message and return
#define __ERROR_OUT__ \
    do { \
        err = errno; \
        close(fd); \
        errno = err; \
        return -1; \
    } while (0);

    int fd, err; // Socket file descriptor
    int reuse = 1; // Socket descriptor reuse

    // Create socket
    if ((fd = socket(addr->sa_family, type, 0)) < 0)
        return -1; // Socket failed, no need to close

    // Set address reuse
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
        __ERROR_OUT__;

    // Bind address and port
    if (bind(fd, addr, alen) < 0)
        __ERROR_OUT__;

    // Listen connect
    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
        if (listen(fd, qlen) < 0) {
            __ERROR_OUT__;
        }
    }
#undef __ERROR_OUT__

    return fd;
}

int main()
{
    struct sockaddr_in lisAddr;
    struct sockaddr_in cliAddr;

    memset(&lisAddr, 0, sizeof(lisAddr));
    lisAddr.sin_family = AF_INET; // IPv4
    lisAddr.sin_port = htons(SERVER_PORT); // Need to be converted to network byte order
    inet_pton(AF_INET, "127.0.0.1", &lisAddr.sin_addr.s_addr);

    int fd;
    // TCP socket
    if ((fd = InitServer(SOCK_STREAM, (struct sockaddr*)&lisAddr, sizeof(lisAddr), 10)) < 0) {
        perror("InitServer");
        exit(EXIT_FAILURE);

    } else {
        socklen_t addrLen = sizeof(cliAddr);
        int newfd;
        if ((newfd = accept(fd, (struct sockaddr*)&cliAddr, &addrLen)) < 0) {
            perror("accept");
            close(fd);
            exit(EXIT_FAILURE);
        } else { // Connection succeeded
            // Receive msg from client
            char buf[5];
            recv(newfd, buf, sizeof(buf), 0);
            cout << buf << endl;

            // Send msg to client
            strncpy(buf, "pong", 5);
            send(newfd, buf, sizeof(buf), 0);

            close(newfd);
        }
    }
    close(fd);

    exit(EXIT_SUCCESS);
}
