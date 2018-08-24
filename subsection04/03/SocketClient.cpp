#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <vector>
#include <list>
#include "SocketCommon.h"
#include "GetDoubleTime.h"

using namespace std;

// Socket descriptor delay required
struct Delayfd_s {
    int fd;
    double startTime;
};

bool operator==(const struct Delayfd_s &ldfd, const struct Delayfd_s &rdfd)
{
    return ldfd.fd == rdfd.fd;
}

// Added to the list of delayed waits
void AddToDelayClose(list<struct Delayfd_s> &delayfds, int fd)
{
    struct Delayfd_s dfd;
    dfd.fd = fd;
    dfd.startTime = GetDoubleTime();
    delayfds.push_back(dfd);
}

// Polling detection ends all delays
int DelayIsOK(list<struct Delayfd_s> &delayfds)
{
    int retNum = 0;
    if (!delayfds.empty()) {
        double curTime = GetDoubleTime();
        for (auto iter = delayfds.begin(); iter != delayfds.end(); iter++) {
            // Not closed and delay time detected has not been reached
            if ((curTime - iter->startTime) < DELAY_TIME) {
                return false;
            } else { // End of delay
                close(iter->fd);
                iter = delayfds.erase(iter);
                retNum++;
            }
        }
    }
    return retNum;
}

// Handle every event of polling
void HandleEvents(int epollfd, struct epoll_event events[], int eventNum,
                 char *buf, size_t len, list<struct Delayfd_s> &delayfds)
{
    for (int i = 0; i < eventNum; i++) {
        if (events[i].events & (EPOLLHUP | EPOLLERR)) {
            close(events[i].data.fd);
            DeleteEpollEvent(epollfd, events[i].data.fd, EPOLLIN|EPOLLOUT);
        } else if (events[i].events & EPOLLIN) {
            if (SOCKET_READ_SUCC == HandleEpollRead(epollfd, events[i].data.fd, buf, len, false)) {
                DeleteEpollEvent(epollfd, events[i].data.fd, EPOLLIN|EPOLLOUT);
                AddToDelayClose(delayfds, events[i].data.fd);
            }
        } else if (events[i].events & EPOLLOUT) {
            HandleEpollWrite(epollfd, events[i].data.fd, buf, true);
        }
    }
}


int main()
{
    struct sockaddr_in sAddr;
    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, LOCAL_IP, &sAddr.sin_addr);

    int epollfd = epoll_create(10);
    struct epoll_event events[CONNECT_NUM];
    char buf[1024] = CLIENT_SEND_STR;
    list<struct Delayfd_s> delayfds;
    double startTime = GetDoubleTime();

    for (int i = 0; i < CONNECT_NUM; i++) {
        int fd;
        // Create socket
        if (0 > (fd = socket(AF_INET, SOCK_STREAM, 0))) {
            perror("socket");
            exit(EXIT_FAILURE);
        } else {
            SetNonBlock(fd);
            AddEpollEvent(epollfd, fd, EPOLLOUT);
        }
        if (0 > connect(fd, (struct sockaddr*)&sAddr, sizeof(sAddr))
            && errno != EINPROGRESS) {
            perror("connect");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    int finishNum = 0;
    for (;;) {
        if (finishNum == CONNECT_NUM) {
            cout << "OK";
            break;
        } else {
            int eventNum;
            if ((eventNum = epoll_wait(epollfd, events, EPOLL_EVENT_NUM,
                    EPOLL_WAIT_TIME))) {
                HandleEvents(epollfd, events, eventNum, buf, sizeof(buf), delayfds);
            }
        }
        finishNum += DelayIsOK(delayfds);
    }

    cout << ", Time:" << GetDoubleTimeDiff(startTime) << endl;

    exit(EXIT_SUCCESS);
}
