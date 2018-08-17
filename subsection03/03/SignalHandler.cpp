#include <signal.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

static void signalMonitor(int signo);

int main()
{
    if (signal(SIGQUIT, signalMonitor) == SIG_ERR) {
        cout << "Can't catch!" << endl;
        exit(1);
    }

    if (signal(SIGTERM, signalMonitor) == SIG_ERR) {
        cout << "Can't catch!" << endl;
        exit(1);
    }

    while (true) {
        cout << "Hello" << endl;
        sleep(1);
    }
}

void signalMonitor(int signo)
{
    if (signo == SIGQUIT) {
        cout << "Bye" << endl;
        exit(0);
    } else if (signo == SIGTERM) {
        cout << "Bye" << endl;
        exit(0);
    } else {
        abort();
    }
}
