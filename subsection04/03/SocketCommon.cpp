//
// Created by fs on 8/21/18.
//

#include <sys/fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <sys/epoll.h>
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include "SocketCommon.h"

using namespace std;

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

void GetEpollEventStr(int event, std::string &out)
{
    if (event & EPOLLIN) {
        out += "[IN]";
    }
    if (event & EPOLLPRI) {
        out += "[PRI]";
    }
    if (event & EPOLLOUT) {
        out += "[OUT]";
    }
    if (event & EPOLLRDNORM) {
        out += "[RDNORM]";
    }
    if (event & EPOLLRDBAND) {
        out += "[RDBAND]";
    }
    if (event & EPOLLWRNORM) {
        out += "[WRNORM]";
    }
    if (event & EPOLLWRBAND) {
        out += "[WRBAND]";
    }
    if (event & EPOLLMSG) {
        out += "[MSG]";
    }
    if (event & EPOLLERR) {
        out += "[ERR]";
    }
    if (event & EPOLLHUP) {
        out += "[HUP]";
    }
    if (event & EPOLLRDHUP) {
        out += "[RDHUP]";
    }
    if (event & EPOLLEXCLUSIVE) {
        out += "[EXCLUSIVE]";
    }
    if (event & EPOLLWAKEUP) {
        out += "[WAKEUP]";
    }
    if (event & EPOLLONESHOT) {
        out += "[ONESHOT]";
    }
    if (event & EPOLLET) {
        out += "[ET]";
    }
}

int AddEpollEvent(int epollfd, int fd, uint32_t state)
{
    struct epoll_event ev{};
    ev.events = state;
    ev.data.fd = fd;
    return epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

int DeleteEpollEvent(int epollfd, int fd, uint32_t state)
{
    struct epoll_event ev{};
    ev.events = state;
    ev.data.fd = fd;
    return epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
}

int ModifyEpollEvent(int epollfd, int fd, uint32_t state)
{
    struct epoll_event ev{};
    ev.events = state;
    ev.data.fd = fd;
    return epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
}

/*
 * Handle epoll read event
 * return value:
 *  1: client close socket
 *  0: read msg success
 * -1: read failed
 */
int HandleEpollRead(int epollfd, int fd, char *buf, size_t len, bool keep)
{
    static int readNum = 0;
    int ret;
    ssize_t nread = read(fd, buf, len);
    if (nread < 0) {
        perror("read");
        close(fd);
        DeleteEpollEvent(epollfd, fd, EPOLLIN);
        ret = -1;
    } else if (nread == 0) {
        printf("fd: %d client close.\n", fd);
        close(fd);
        DeleteEpollEvent(epollfd, fd, EPOLLIN);
        ret = 1;
    } else {
        cout << ++readNum << "read:" << buf << endl;
        if (keep) {
            ModifyEpollEvent(epollfd, fd, EPOLLOUT);
        }
        ret = 0;
    }
    return ret;
}

int HandleEpollWrite(int epollfd, int fd, char *buf, bool keep)
{
    int ret;
    string str(buf + std::to_string(fd));
    ssize_t nwrite = write(fd, str.c_str(), str.size()+1);
    if (nwrite < 0) {
        perror("write");
        close(fd);
        DeleteEpollEvent(epollfd, fd, EPOLLOUT);
        ret = 1;
    } else {
        cout << "write succeed" << endl;
        if (keep) {
            ModifyEpollEvent(epollfd, fd, EPOLLIN);
        }
        ret = 0;
    }
    return ret;
}

int HandleEpollAccept(struct epoll_event *event, int epollfd, int listenfd, bool keep)
{
    int ret;
    struct sockaddr_in cliaddr{};
    socklen_t cliaddrlen = sizeof(struct sockaddr_in);

    int clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
    if (clifd == -1) {
        perror("accpet");
        ret = -1;
    } else {
        printf("accept a new client%d: %s:%d\n",
               event->data.u32, inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
        if (keep) {
            AddEpollEvent(epollfd, clifd, EPOLLIN);
        }
        ret = 0;
    }
    return ret;
}
