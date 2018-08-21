#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "Common.h"
#include "GetFloatTime.h"

using namespace std;

void *ThreadFunc(void *arg)
{
    int fd = *(int*)arg;
    sleep(10);
    close(fd);
    cout << fd << "关闭！" << endl;
    return arg;
}

int main()
{
    struct sockaddr_in serAddr{};
    pthread_t pthreadIDs[CONNECT_NUM];
    int fds[CONNECT_NUM];

    for (int i = 0; i < CONNECT_NUM; i++) {
        // Create socket
        fds[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (fds[i] < 0) {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        serAddr.sin_family = AF_INET; // IPv4
        serAddr.sin_port = htons(SERVER_PORT);
        inet_pton(AF_INET, "127.0.0.1", &serAddr.sin_addr); // Local

        // Connect failed
        if (connect(fds[i], (struct sockaddr*)&serAddr, sizeof(serAddr)) < 0) {
            close(fds[i]);
            perror("connect");
            exit(EXIT_FAILURE);

        } else { // Connect succeed
            // Send msg to server
            char buf[] = "ping";
            write(fds[i], buf, sizeof(buf));

            // Receive msg from server
            read(fds[i], buf, sizeof(buf));
            cout << i << ",fd: " << fds[i] << buf << "time: " << GetIntTime() << endl;
            int error = pthread_create(&pthreadIDs[i], nullptr, ThreadFunc, &fds[i]);
            if (error != 0) {
                cout << "pthread_create:" << strerror(error) << endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    exit(EXIT_SUCCESS);
}
