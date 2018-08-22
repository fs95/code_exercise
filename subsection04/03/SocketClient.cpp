#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/fcntl.h>
#include <poll.h>
#include "SocketCommon.h"
#include "GetDoubleTime.h"

using namespace std;

// Task state
enum TaskState_e {
    STATE_WR,
    STATE_WAIT,
    STATE_EXIT,
};

// 1000 task information
struct TaskInfo_s {
    int fd;
    enum TaskState_e state;
    double time;
} taskInfos[CONNECT_NUM];

struct pollfd pollfds[CONNECT_NUM];

const char sendStr[] = CLIENT_SEND_STR;
char recvBuf[512];

bool ClientStateMachine(struct TaskInfo_s *_taskInfo, struct pollfd *fdPoll,
        const char *sendBuf, char *recvBuf, size_t bufLen)
{
    if (fdPoll->fd >= 0 && fdPoll->revents & SOCKET_POLLOUT) {
        // Send msg to server
        string str(sendBuf + std::to_string(_taskInfo->fd));
        if (write(fdPoll->fd, str.c_str(), str.size()) <= 0) {
//            perror("write");
        } else { // Write successful
            cout << "Write success!" << fdPoll->fd << endl;
            fdPoll->events = SOCKET_POLLIN;
        }
    } else if (fdPoll->fd >= 0 && fdPoll->revents & SOCKET_POLLIN) {
        // Receive msg from server
        if ((read(fdPoll->fd, recvBuf, bufLen)) < 0 && errno == EAGAIN) {
            cout << errno << endl;
            perror("read");
        } else { // Read successful
            cout << "Read:" << recvBuf << endl;
            fdPoll->fd = -1; // Ignore
            _taskInfo->state = STATE_WAIT; // Update state
            _taskInfo->time = GetDoubleTime(); // Get current time
        }
    } else if (_taskInfo->state == STATE_WAIT
    && GetDoubleTimeDiff(_taskInfo->time) >= DELAY_TIME) {
        // 10 seconds waiting to be completed!
        // Close socket and exit
        close(_taskInfo->fd);
        _taskInfo->state = STATE_EXIT;
    }

    return (_taskInfo->state == STATE_EXIT);
}

int main()
{
    double startTime = GetDoubleTime(); // Get current time

    struct sockaddr_in sAddr{};
    sAddr.sin_family = AF_INET; // IPv4
    sAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &sAddr.sin_addr); // Local

    for (int i = 0; i < CONNECT_NUM; i++) {
        // Create socket
        if (0 > (taskInfos[i].fd = socket(AF_INET, SOCK_STREAM, 0))) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        // Init
        SetNonBlock(taskInfos[i].fd);
        pollfds[i].fd = taskInfos[i].fd;
        pollfds[i].events = SOCKET_POLLOUT;
        pollfds[i].revents = SOCKET_POLLOUT;
        taskInfos[i].state = STATE_WR;

        // Connect failed
        if (connect(taskInfos[i].fd, (struct sockaddr*)&sAddr, sizeof(sAddr)) < 0
            && errno != EINPROGRESS) {
            close(taskInfos[i].fd);
            perror("connect");
            exit(EXIT_FAILURE);
        } else { // Connect succeed
            ClientStateMachine(&taskInfos[i], &pollfds[i], sendStr,
                    recvBuf, sizeof(recvBuf));
        }
    }

    bool needPoll = true;
    while (true) {
        bool quit = true;
        if (needPoll) {
            cout << "Poll: " << poll(pollfds, CONNECT_NUM, -1) << endl;
            needPoll = false;
        }
        for (int i = 0; i < CONNECT_NUM; i++) {
            struct pollfd &pfd = pollfds[i];

            if (!ClientStateMachine(&taskInfos[i], &pollfds[i], sendStr,
                    recvBuf, sizeof(recvBuf))) {
                quit = false;
            }
            if (pfd.fd != -1) {
                needPoll = true;
            }
        }
        if (quit) {
            break;
        }
    }
    cout << "Finish! Time consuming:" << GetDoubleTimeDiff(startTime) << endl;

    exit(EXIT_SUCCESS);
}
