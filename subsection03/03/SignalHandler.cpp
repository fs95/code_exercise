#include <signal.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

static void SignalMonitor(int signo);

int main()
{
    if (signal(SIGQUIT, SignalMonitor) == SIG_ERR) {
        cout << "Can't catch!" << endl;
        exit(1);
    }

    if (signal(SIGTERM, SignalMonitor) == SIG_ERR) {
        cout << "Can't catch!" << endl;
        exit(1);
    }

    while (true) {
        cout << "Hello" << endl;
        sleep(1);
    }
}

void SignalMonitor(int signo)
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
