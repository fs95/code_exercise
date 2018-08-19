/*
 * The main thread creates 4 new threads, 4 new threads simultaneously do a
 * self-increment operation on several of shared variables, each new thread
 * does 1e+9 times on the shared variable of the operation to be operated to
 * be operated self-increment.
 */

#include <iostream>
#include <pthread.h>
#include <cstring>
#include <stdlib.h>

using namespace std;

#define SELF_INC_NUM 1e+7 // The number of increments pre variable

//#define USE_LOCK_TYPE_MUTEX // Use thread mutex
#define USE_LOCK_TYPE_SPIN // Use thread spin lock

typedef void *(*ThreadFunc_t)(void*); // Thread function type

// Variable thread lock
struct VarLock_s {
    int64_t *const ptr; // Variable pointer
    pthread_mutex_t mutexLock; // Use if define USE_LOCK_TYPE_MUTEX
    pthread_spinlock_t spinLock; // Use if define USE_LOCK_TYPE_SPIN
};

int64_t p = 0;  // For the 1/2/3 threads, the result is 3e+9
int64_t q = 0;  // For the 1/2/4 threads, the result is 3e+9
int64_t r = 0;  // For the 1/3 threads, the result is 2e+9
int64_t s = 0;  // For the 1/2/3/4 threads, the result is 4e+9

// Variable and lock
struct VarLock_s pVarLock = {&p, PTHREAD_MUTEX_INITIALIZER};
struct VarLock_s qVarLock = {&q, PTHREAD_MUTEX_INITIALIZER};
struct VarLock_s rVarLock = {&r, PTHREAD_MUTEX_INITIALIZER};
struct VarLock_s sVarLock = {&s, PTHREAD_MUTEX_INITIALIZER};

// Variable increment and lock detection
#define INC_MACRO(remain,varLock_p) \
    if (remain && IncVarLock(varLock_p)) { \
        remain--; \
    };

// Try lock a lock and increment variable
bool IncVarLock(struct VarLock_s *varLock_p)
{

#ifdef USE_LOCK_TYPE_MUTEX
    int rtn = pthread_mutex_trylock(&varLock_p->mutexLock);
#else
    int rtn = pthread_spin_trylock(&varLock_p->spinLock);
#endif

    if (rtn == 0) { // Lock successfully
        (*varLock_p->ptr)++;

#ifdef USE_LOCK_TYPE_MUTEX
        pthread_mutex_unlock(&varLock_p->mutexLock);
#else
        pthread_spin_unlock(&varLock_p->spinLock);
#endif
        return true;

    } else { // The variable is locked by another thread
        return false;
    }
}

// Thread 1 run function
void *ThreadFunc1(void *)
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

    return ((void*)1);
}

// Thread 2 run function
void *ThreadFunc2(void *)
{
    auto pr = static_cast<int64_t>(SELF_INC_NUM);
    auto qr = static_cast<int64_t>(SELF_INC_NUM);
    auto sr = static_cast<int64_t>(SELF_INC_NUM);

    while (pr || qr || sr) {
        INC_MACRO(pr,&pVarLock);
        INC_MACRO(qr,&qVarLock);
        INC_MACRO(sr,&sVarLock);
    }

    return ((void*)2);
}

// Thread 3 run function
void *ThreadFunc3(void *)
{
    auto pr = static_cast<int64_t>(SELF_INC_NUM);
    auto rr = static_cast<int64_t>(SELF_INC_NUM);
    auto sr = static_cast<int64_t>(SELF_INC_NUM);

    while (pr || rr || sr) {
        INC_MACRO(pr,&pVarLock);
        INC_MACRO(rr,&rVarLock);
        INC_MACRO(sr,&sVarLock);
    }

    return ((void*)3);
}

// Thread 4 run function
void *ThreadFunc4(void *)
{
    auto qr = static_cast<int64_t>(SELF_INC_NUM);
    auto sr = static_cast<int64_t>(SELF_INC_NUM);

    while (qr || sr) {
        INC_MACRO(qr,&qVarLock);
        INC_MACRO(sr,&sVarLock);
    }

    return ((void*)4);
}

int main()
{
    int error;
    void *threadReturn; // Receive thread return value
    pthread_t pthreadIDs[4];
    ThreadFunc_t threadFuncs[4] = {
            ThreadFunc1,
            ThreadFunc2,
            ThreadFunc3,
            ThreadFunc4
    };

#ifdef USE_LOCK_TYPE_SPIN
    // Thread spin lock init
    pthread_spin_init(&pVarLock.spinLock, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init(&qVarLock.spinLock, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init(&rVarLock.spinLock, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init(&sVarLock.spinLock, PTHREAD_PROCESS_PRIVATE);
#endif

    // Get current time
    timespec tsp1{};
    clock_gettime(CLOCK_REALTIME, & tsp1); // Get high precision UTC time

    // Create thread
    for (int i = 0; i < 4; i++) {
        error = pthread_create(&pthreadIDs[i], nullptr, threadFuncs[i], nullptr);
        if (error != 0) {
            cout << "pthread_create:" << strerror(error) << endl;
            exit(EXIT_FAILURE);
        }
    }

    // Wait thread
    for (int i = 0; i < 4; i++) {
        error = pthread_join(pthreadIDs[i], &threadReturn);
        if (error != 0) {
            cout << "pthread_join" << i+1 << strerror(error) << endl;
            exit(EXIT_FAILURE);
        }
    }

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
