//
// Created by fs on 8/21/18.
//

#ifndef SOCKET_COMMON_H
#define SOCKET_COMMON_H

#include <netinet/in.h>
#include <poll.h>

#define SERVER_PORT (8192)
#define CONNECT_NUM (700)
#define DELAY_TIME (1)

bool SetNonBlock(int fd);

#define SOCKET_POLLIN (POLLIN|POLLPRI)
#define SOCKET_POLLOUT (POLLWRBAND|POLLOUT)

int InitServer(int type, const struct sockaddr *addr, socklen_t alen, int qlen);

#define CLIENT_SEND_STR "ping"
#define SERVER_SEND_STR "pong"

#endif //SOCKET_COMMON_H
