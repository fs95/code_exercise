/*
 * The main thread creates 4 new threads, 4 new threads simultaneously do a
 * self-increment operation on several of shared variables, each new thread
 * does 1e+9 times on the shared variable of the operation to be operated to
 * be operated self-increment.
 */

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;

#define SELF_INC_NUM ((int64_t)1e+7) // The number of increments pre variable

typedef void (*ThreadFunc_t)(void*); // Thread function type

struct Arguments_s {
    int argc;
    void **argv;
};

atomic<int64_t> p = {0};  // For the 1/2/3 threads, the result is 3e+9
atomic<int64_t> q = {0};  // For the 1/2/4 threads, the result is 3e+9
atomic<int64_t> r = {0};  // For the 1/3 threads, the result is 2e+9
atomic<int64_t> s = {0};  // For the 1/2/3/4 threads, the result is 4e+9

void ThreadFunc(void *ptr)
{
    auto *arg = (struct Arguments_s*)ptr;

    for (int64_t j = 0; j < SELF_INC_NUM; j++) {
        for (int i = 0; i < arg->argc; i++) {
            (*(atomic<int64_t> *) arg->argv[i])++;
        }
    }
}

int main()
{
    // Get current time
    timespec tsp1;
    clock_gettime(CLOCK_REALTIME, & tsp1); // Get high precision UTC time
    thread ths[4];
    struct Arguments_s argus[4];

    // Create thread
    argus[0].argc = 4;
    argus[0].argv = (void **)malloc(argus[0].argc * sizeof(void*));
    argus[0].argv[0] = &p;
    argus[0].argv[1] = &q;
    argus[0].argv[2] = &r;
    argus[0].argv[3] = &s;

    argus[1].argc = 3;
    argus[1].argv = (void **)malloc(argus[1].argc * sizeof(void*));
    argus[1].argv[0] = &p;
    argus[1].argv[1] = &q;
    argus[1].argv[2] = &s;

    argus[2].argc = 3;
    argus[2].argv = (void **)malloc(argus[2].argc * sizeof(void*));
    argus[2].argv[0] = &p;
    argus[2].argv[1] = &r;
    argus[2].argv[2] = &s;

    argus[3].argc = 2;
    argus[3].argv = (void **)malloc(argus[3].argc * sizeof(void*));
    argus[3].argv[0] = &q;
    argus[3].argv[1] = &s;

    for(int i = 0; i < 4; i++)
    {
        ths[i] = thread(&ThreadFunc, &argus[i]);
    }

    for(int i = 0; i < 4; i++)
    {
        ths[i].join();
    }

    // Time spent
    timespec tsp2;
    clock_gettime(CLOCK_REALTIME, &tsp2); // Get high precision UTC time
    float timeConsume = tsp2.tv_sec - tsp1.tv_sec
            + (tsp2.tv_nsec - tsp1.tv_nsec) / 1e9f;
    cout << "time:" << timeConsume << "s" << endl;
    cout << "    p:" << p << endl;
    cout << "    q:" << q << endl;
    cout << "    r:" << r << endl;
    cout << "    s:" << s << endl;

    exit(EXIT_SUCCESS);
}
