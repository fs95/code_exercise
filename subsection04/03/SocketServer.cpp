#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include "SocketCommon.h"

using namespace std;

// Task state
enum STaskState_e {
    STATE_CONNECT,
    STATE_WR,
    STATE_EXIT,
};

struct STaskInfo_s {
    int fd;
    enum STaskState_e state;
} taskInfos[CONNECT_NUM];

struct pollfd pollfds[CONNECT_NUM+1];
//struct pollfd lispollfd; // Listen fd

const char sendStr[] = SERVER_SEND_STR;
char recvBuf[512];

bool ServerStateMachine(int fd, struct STaskInfo_s *_taskInfo, struct pollfd *lisfdPoll, struct pollfd *sockfdPoll,
        const char *sendBuf, char *recvBuf, size_t bufLen)
{
    if (_taskInfo->state == STATE_CONNECT && lisfdPoll->fd >= 0
    && lisfdPoll->revents & SOCKET_POLLIN) {
        if ((_taskInfo->fd = accept(fd, nullptr, nullptr)) <= 0) {
//            perror("accept");
        } else {
            sockfdPoll->fd = _taskInfo->fd;
            cout << "Connect success!" << sockfdPoll->fd << endl;
            sockfdPoll->events = SOCKET_POLLIN;
            _taskInfo->state = STATE_WR;
        }
    }

    if (_taskInfo->state == STATE_WR && sockfdPoll->fd > 0
    && sockfdPoll->revents & SOCKET_POLLIN) {
        // Receive msg from server
        if ((read(sockfdPoll->fd, recvBuf, bufLen)) < 0 && errno == EAGAIN) {
//            perror("read");
        } else { // Read successful
            cout << "Read:" << recvBuf << endl;
            sockfdPoll->events = SOCKET_POLLOUT;
        }
    } else if (_taskInfo->state == STATE_WR && sockfdPoll->fd > 0
    && sockfdPoll->revents & SOCKET_POLLOUT) {
        // Send msg to server
        string str(sendBuf + std::to_string(sockfdPoll->fd));
        if (write(sockfdPoll->fd, str.c_str(), str.size()) <= 0) {
//            perror("write");
        } else { // Write successful
            cout << "Write successful!" << sockfdPoll->fd << endl;
            sockfdPoll->fd = -1; // Ignore
            _taskInfo->state = STATE_EXIT;
            // TODO close(fd)
        }
    }
    return _taskInfo->state == STATE_EXIT;
}

int main()
{
    struct sockaddr_in lisAddr{};
    struct sockaddr_in cliAddr{};

    memset(&lisAddr, 0, sizeof(lisAddr));
    lisAddr.sin_family = AF_INET; // IPv4
    lisAddr.sin_port = htons(SERVER_PORT); // Converted to network byte order
    inet_pton(AF_INET, "127.0.0.1", &lisAddr.sin_addr.s_addr);

    int fd;
    // TCP socket
    if ((fd = InitServer(SOCK_STREAM, (struct sockaddr*)&lisAddr, sizeof(lisAddr), 10)) < 0) {
        perror("InitServer");
        exit(EXIT_FAILURE);
    }

    // Init
    SetNonBlock(fd);
    pollfds[CONNECT_NUM].fd = fd;
    pollfds[CONNECT_NUM].events = SOCKET_POLLIN;
    pollfds[CONNECT_NUM].revents = POLLIN;
    for (int i = 0; i < CONNECT_NUM; i++) {
        taskInfos[i].state = STATE_CONNECT;
        pollfds[i].fd = -1; // Ignore
        pollfds[i].events = SOCKET_POLLIN;
        pollfds[i].revents = POLLIN;
    }

    socklen_t addrLen = sizeof(cliAddr);
    int newfds[CONNECT_NUM];
    bool needPoll = false;
    while (true) {
        bool quit = true;
        if (needPoll) {
            poll(pollfds, CONNECT_NUM+1, -1);
            needPoll = false;
        }
        for (int i = 0; i < CONNECT_NUM; i++) {

            if (!ServerStateMachine(fd, &taskInfos[i], &pollfds[CONNECT_NUM], &pollfds[i], sendStr,
                               recvBuf, sizeof(recvBuf))) {
                quit = false;
            }
        }
        for (auto &taskInfo : taskInfos) {
            if (taskInfo.state != STATE_EXIT) {
                needPoll = true;
                break;
            }
        }
        for (int i = 0; i < CONNECT_NUM; i++) {
            if (pollfds[i].fd > 0) {
                quit = false;
                break;
            }
        }
        if (quit) {
            break;
        }
    }
    close(fd);

    exit(EXIT_SUCCESS);
}
