//
// Created by fs on 8/21/18.
//

#ifndef SOCKET_COMMON_H
#define SOCKET_COMMON_H

#include <netinet/in.h>
#include <poll.h>
#include <string>

#define LOCAL_IP "127.0.0.1"
#define SERVER_PORT (8192)
#define CONNECT_NUM (1000)
#define DELAY_TIME (10)
#define EPOLL_EVENT_NUM (500)
#define EPOLL_WAIT_TIME (100)

// Handle epoll read state
#define SOCKET_CLOSED (1)
#define SOCKET_READ_SUCC (0)
#define SOCKET_READ_FAILED (-1)

bool SetNonBlock(int fd);
int InitServer(int type, const struct sockaddr *addr, socklen_t alen, int qlen);
void GetEpollEventStr(int event, std::string &out);
int AddEpollEvent(int epollfd, int fd, uint32_t state);
int DeleteEpollEvent(int epollfd, int fd, uint32_t state);
int ModifyEpollEvent(int epollfd, int fd, uint32_t state);
int HandleEpollRead(int epollfd, int fd, char *buf, size_t len, bool keep);
int HandleEpollWrite(int epollfd, int fd, char *buf, bool keep);
void HandleEpollAccept(int epollfd, int listenfd, bool keep);

#define CLIENT_SEND_STR "ping"
#define SERVER_SEND_STR "pong"

#endif //SOCKET_COMMON_H
