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

int main()
{
    int fd;
    struct sockaddr_in serAddr;

    // Create socket
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    serAddr.sin_family = AF_INET; // IPv4
    serAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &serAddr.sin_addr); // Local

    // Connect failed
    if (connect(fd, (struct sockaddr*)&serAddr, sizeof(serAddr)) < 0) {
        close(fd);
        perror("connect");
        exit(EXIT_FAILURE);

    } else { // Connect succeed
        // Send msg to server
        char buf[5];
        strncpy(buf, "ping", 5);
        send(fd, buf, sizeof(buf), 0);

        // Receive msg from server
        recv(fd, buf, sizeof(buf), 0);
        cout << buf << endl;

        close(fd);
    }

    exit(EXIT_SUCCESS);
}
