/*
 * Practice the use of the message queue, the parent process sends a message
 * to child process through the message queue, and the child process send a
 * message to the parent process after receiving the message.
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <errno.h>

using namespace std;

#define MAX_TEXT 100 // Message maximum byte
#define PARENT_TYPE 50 // Type and content sent by parent process
#define PARENT_TEXT "ABC"
#define CHILD_TYPE 100 // Type and content sent by child process
#define CHILD_TEXT "DEF"
#define DEBUG 0

// Message queue message item
typedef struct {
    long msgType;
    char msgText[MAX_TEXT];
} MsgBuf_t;

void ParentProcess(void *arg);
void ChildProcess(void *arg);

int main()
{
    // Create or open a message queue
    key_t msgKey = ftok(".", '0'); //Create an IPC key
    int msgId = msgget(msgKey, 0666 | IPC_EXCL | IPC_CREAT);
    if (msgId == -1) {
        if (errno == EEXIST) { // Message queue already exists
            msgId = msgget(msgKey, 0666);
            cout << "[DEBUG]Open message queue successfully, msqid:" << msgId << endl;

        } else {
            perror("msgget");
            exit(EXIT_FAILURE);
        }
    } else { // Create successfully
        cout << "[DEBUG]Create message queue successfully, msqid:" << msgId << endl;
    }

    // Create child process
    pid_t pid = fork();
    if (pid < 0) { // Error
        perror("fork");
        exit(EXIT_FAILURE);

    } else if (pid > 0) { // Parent
        ParentProcess(&msgId);

    } else { // Child
        ChildProcess(&msgId);
    }

    exit(EXIT_SUCCESS);
}

// Parent Process
void ParentProcess(void *arg)
{
    int msgId = *((int*)arg); // Message ID
    MsgBuf_t msgBuf = { // Message buffer
            .msgType = PARENT_TYPE, PARENT_TEXT};

    if (msgsnd(msgId, &msgBuf, MAX_TEXT, 0) == -1) { // Send message failed
        perror("[Parent]msgsnd");
        exit(EXIT_FAILURE);

    } else { // Send successfully
        if (msgrcv(msgId, &msgBuf, MAX_TEXT, CHILD_TYPE, 0) == -1) { // Receive failed
            perror("[Parent]msgrcv");
            exit(EXIT_FAILURE);

        } else if (strcmp(msgBuf.msgText, CHILD_TEXT) == 0) { // Receive successfully and message match
            cout << "parent success" << endl;

        } else { // Receive successfully but does not match
            cout << "[Parent]The data received is not " << CHILD_TEXT << endl;
        }

        // Get message queue information
        msqid_ds msqidDs;
        msgctl(msgId, IPC_STAT, &msqidDs);

#if DEBUG == 1
        // Clear message queue
        while (msqidDs.msg_qnum) {
            if (msgrcv(msgId, &msgBuf, MAX_TEXT, 0, 0) == -1) {
                perror("[Parent]msgrcv");
                exit(EXIT_FAILURE);

            } else {
                cout << "[Parent]Read all the rest:" << msgBuf.msgText << endl;
            }
        }
#endif

        msgctl(msgId, IPC_RMID, &msqidDs); // Close message queue
        cout << "[DEBUG]Message queue has been deleted." << endl;
    }
    exit(EXIT_SUCCESS);
}

// Child Process
void ChildProcess(void *arg)
{
    int msgId = *((int*)arg); // Message ID
    MsgBuf_t msgBuf; // Message buffer

    if (msgrcv(msgId, &msgBuf, MAX_TEXT, PARENT_TYPE, 0) == -1) { // Receive failed
        perror("[Child]msgrcv");
        exit(EXIT_FAILURE);

    } else if (strcmp(msgBuf.msgText, PARENT_TEXT) == 0) { // Receive successfully and message match
        cout << "child success" << endl;
        msgBuf.msgType = CHILD_TYPE;
        strncpy(msgBuf.msgText, CHILD_TEXT, strlen(CHILD_TEXT)+1);

        if (msgsnd(msgId, &msgBuf, MAX_TEXT, 0) == -1) { // Send failed
            perror("[Child]msgsnd");
            exit(EXIT_FAILURE);
        }
    } else { // Receive successfully but does not match
        cout << "[Child]The data received is not " << PARENT_TEXT << endl;
    }

    exit(EXIT_SUCCESS);
}
