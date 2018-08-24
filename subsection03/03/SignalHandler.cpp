#include <csignal>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

using namespace std;

static void SignalMonitor(int signo);

int main()
{
    if (signal(SIGQUIT, SignalMonitor) == SIG_ERR) {
        cout << "Can't catch!" << endl;
        exit(EXIT_FAILURE);
    }

    if (signal(SIGTERM, SignalMonitor) == SIG_ERR) {
        cout << "Can't catch!" << endl;
        exit(EXIT_FAILURE);
    }

    if (signal(SIGALRM, SignalMonitor) == SIG_ERR) {
        cout << "Can't catch!" << endl;
        exit(EXIT_FAILURE);
    }
    while (true) {
        alarm(1);
        pause();
    }
}

void SignalMonitor(int signo)
{
    if (signo == SIGQUIT) {
        cout << "Bye" << endl;
        exit(EXIT_SUCCESS);
    } else if (signo == SIGTERM) {
        cout << "Bye" << endl;
        exit(EXIT_SUCCESS);
    } else if (signo == SIGALRM) {
        cout << "Hello" << endl;
    } else {
        abort();
    }
}
