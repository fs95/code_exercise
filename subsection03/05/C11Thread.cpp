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

using namespace std;

#define SELF_INC_NUM 1e+7 // The number of increments pre variable

typedef void (*ThreadFunc_t)(void*); // Thread function type

// Variable thread lock
struct VarLock_s {
    int64_t *const ptr; // Variable pointer
    mutex c11MutexLock; // C11 thread mutex
};

int64_t p = 0;  // For the 1/2/3 threads, the result is 3e+9
int64_t q = 0;  // For the 1/2/4 threads, the result is 3e+9
int64_t r = 0;  // For the 1/3 threads, the result is 2e+9
int64_t s = 0;  // For the 1/2/3/4 threads, the result is 4e+9

// Variable and lock
struct VarLock_s pVarLock = {&p};
struct VarLock_s qVarLock = {&q};
struct VarLock_s rVarLock = {&r};
struct VarLock_s sVarLock = {&s};

// Variable increment and lock detection
#define INC_MACRO(remain,varLock_p) \
    do { \
        if (remain && IncVarLock(varLock_p)) { \
            remain--; \
        }; \
    } while (0);

// Try lock a lock and increment variable
bool IncVarLock(struct VarLock_s *varLock_p)
{
    if (varLock_p->c11MutexLock.try_lock()) { // Lock successfully
        (*varLock_p->ptr)++;
        varLock_p->c11MutexLock.unlock();
        return true;

    } else { // The variable is locked by another thread
        return false;
    }
}

// Thread 1 run function
void ThreadFunc1(void *)
{
    // Remaining increment
    auto pr = static_cast<int64_t>(SELF_INC_NUM);
    auto qr = static_cast<int64_t>(SELF_INC_NUM);
    auto rr = static_cast<int64_t>(SELF_INC_NUM);
    auto sr = static_cast<int64_t>(SELF_INC_NUM);

    while (pr || qr || rr || sr) {
        INC_MACRO(pr,&pVarLock);
        INC_MACRO(qr,&qVarLock);
        INC_MACRO(rr,&rVarLock);
        INC_MACRO(sr,&sVarLock);
    }
}

// Thread 2 run function
void ThreadFunc2(void *)
{
    auto pr = static_cast<int64_t>(SELF_INC_NUM);
    auto qr = static_cast<int64_t>(SELF_INC_NUM);
    auto sr = static_cast<int64_t>(SELF_INC_NUM);

    while (pr || qr || sr) {
        INC_MACRO(pr,&pVarLock);
        INC_MACRO(qr,&qVarLock);
        INC_MACRO(sr,&sVarLock);
    }
}

// Thread 3 run function
void ThreadFunc3(void *)
{
    auto pr = static_cast<int64_t>(SELF_INC_NUM);
    auto rr = static_cast<int64_t>(SELF_INC_NUM);
    auto sr = static_cast<int64_t>(SELF_INC_NUM);

    while (pr || rr || sr) {
        INC_MACRO(pr,&pVarLock);
        INC_MACRO(rr,&rVarLock);
        INC_MACRO(sr,&sVarLock);
    }
}

// Thread 4 run function
void ThreadFunc4(void *)
{
    auto qr = static_cast<int64_t>(SELF_INC_NUM);
    auto sr = static_cast<int64_t>(SELF_INC_NUM);

    while (qr || sr) {
        INC_MACRO(qr,&qVarLock);
        INC_MACRO(sr,&sVarLock);
    }
}

int main()
{
    // Get current time
    timespec tsp1{};
    clock_gettime(CLOCK_REALTIME, & tsp1); // Get high precision UTC time

    // Create thread
    thread thread1(ThreadFunc1, nullptr);
    thread thread2(ThreadFunc2, nullptr);
    thread thread3(ThreadFunc3, nullptr);
    thread thread4(ThreadFunc4, nullptr);

    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();

    // Time spent
    timespec tsp2{};
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
