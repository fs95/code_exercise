#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include "SocketCommon.h"

using namespace std;

int SocketListenNonBlc(const char *addr, uint16_t port, int listen)
{
    int listenfd;
    struct sockaddr_in lisAddr;

    memset(&lisAddr, 0, sizeof(lisAddr));
    lisAddr.sin_family = AF_INET; // IPv4
    lisAddr.sin_port = htons(port); // Need to be converted to network byte order
    inet_pton(AF_INET, addr, &lisAddr.sin_addr.s_addr);

    // TCP socket
    if ((listenfd = InitServer(SOCK_STREAM, (struct sockaddr*)&lisAddr,
            sizeof(lisAddr), listen)) < 0) {
        perror("InitServer");
        return -1;
    }

    SetNonBlock(listenfd);

    return listenfd;
}

int HandleEvents(int epollfd, struct epoll_event events[], int eventNum,
                  int listenfd, char *buf, size_t len)
{
    int retOkNum = 0;
    for (int i = 0; i < eventNum; i++) {
        if (events[i].data.fd == listenfd && events[i].events & EPOLLIN) {
            HandleEpollAccept(epollfd, listenfd, true);
        } else if (events[i].events & (EPOLLHUP | EPOLLERR)) {
            close(events[i].data.fd);
            DeleteEpollEvent(epollfd, events[i].data.fd, EPOLLIN|EPOLLOUT);
        } else if (events[i].events & EPOLLIN) {
            if (SOCKET_CLOSED == HandleEpollRead(epollfd, events[i].data.fd, buf, len, true)) {
                retOkNum++;
            }
        } else if (events[i].events & EPOLLOUT) {
            HandleEpollWrite(epollfd, events[i].data.fd, buf, true);
        }
    }
    return retOkNum;
}

int main()
{
    int listenfd = SocketListenNonBlc(LOCAL_IP, SERVER_PORT, CONNECT_NUM);
    if (listenfd < 0) {
        exit(EXIT_FAILURE);
    }
    struct epoll_event events[EPOLL_EVENT_NUM];
    int epollfd = epoll_create(EPOLL_EVENT_NUM);
    char buf[1024] = SERVER_SEND_STR;
    int completionN = 0;

    AddEpollEvent(epollfd, listenfd, EPOLLIN);

    int eventNum;
    for (;;) {
        if ((eventNum = epoll_wait(epollfd, events, EPOLL_EVENT_NUM,
                EPOLL_WAIT_TIME))) {
            completionN += HandleEvents(epollfd, events, eventNum, listenfd,
                    buf, sizeof(buf));
            if (completionN == CONNECT_NUM) {
                break;
            }
        }
    }

    close(epollfd);
    close(listenfd);
    exit(EXIT_SUCCESS);
}
