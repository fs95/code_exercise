//
// Created by fs on 8/21/18.
//

#include <sys/fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include "SocketCommon.h"

bool SetNonBlock(int fd)
{
    // Set non-block
    int flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flag) == 0;
}

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
