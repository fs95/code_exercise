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

#define SELF_INC_NUM ((int64_t)1e+7) // The number of increments pre variable

//#define USE_LOCK_TYPE_MUTEX // Use thread mutex
#define USE_LOCK_TYPE_SPIN // Use thread spin lock

typedef void *(*ThreadFunc_t)(void*); // Thread function type

// Variable thread lock
struct VarLock_s {
    int64_t *const ptr; // Variable pointer
#ifdef USE_LOCK_TYPE_MUTEX
    pthread_mutex_t mutexLock; // Use if define USE_LOCK_TYPE_MUTEX
#else
    pthread_spinlock_t spinLock; // Use if define USE_LOCK_TYPE_SPIN
#endif
};

struct Arguments_s {
    int argc;
    void **argv;
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
#define INC_MACRO(remain_p,varLock_p) \
    do { \
        if ((*remain_p) && IncVarLock(varLock_p)) { \
            (*remain_p)--; \
        }; \
    } while (0);

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

void *ThreadFunc(void *ptr)
{
    auto *arg = (struct Arguments_s*)ptr;
    int64_t num[arg->argc];
    for (auto &n : num)
        n = SELF_INC_NUM;

    while (true) {
        int64_t Continue = 0;

        for (int i = 0; i < arg->argc; i++) {
            INC_MACRO(&num[i], (struct VarLock_s*)arg->argv[i]);
            Continue |= num[i];
        }
        if (0 == Continue)
            break;
    }

    return nullptr;
}

int main()
{
    int error;
    void *threadReturn; // Receive thread return value
    pthread_t pthreadIDs[4];
    struct Arguments_s argus[4];

#ifdef USE_LOCK_TYPE_MUTEX
    // Thread spin lock init
    pthread_mutex_init(&pVarLock.mutexLock, nullptr);
    pthread_mutex_init(&qVarLock.mutexLock, nullptr);
    pthread_mutex_init(&rVarLock.mutexLock, nullptr);
    pthread_mutex_init(&sVarLock.mutexLock, nullptr);
#else
    // Thread spin lock init
    pthread_spin_init(&pVarLock.spinLock, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init(&qVarLock.spinLock, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init(&rVarLock.spinLock, PTHREAD_PROCESS_PRIVATE);
    pthread_spin_init(&sVarLock.spinLock, PTHREAD_PROCESS_PRIVATE);
#endif

    // Get current time
    timespec tsp1;
    clock_gettime(CLOCK_REALTIME, & tsp1); // Get high precision UTC time

    // Create thread
    argus[0].argc = 4;
    argus[0].argv = (void **)malloc(argus[0].argc * sizeof(void*));
    argus[0].argv[0] = &pVarLock;
    argus[0].argv[1] = &qVarLock;
    argus[0].argv[2] = &rVarLock;
    argus[0].argv[3] = &sVarLock;

    argus[1].argc = 3;
    argus[1].argv = (void **)malloc(argus[1].argc * sizeof(void*));
    argus[1].argv[0] = &pVarLock;
    argus[1].argv[1] = &qVarLock;
    argus[1].argv[2] = &sVarLock;

    argus[2].argc = 3;
    argus[2].argv = (void **)malloc(argus[2].argc * sizeof(void*));
    argus[2].argv[0] = &pVarLock;
    argus[2].argv[1] = &rVarLock;
    argus[2].argv[2] = &sVarLock;

    argus[3].argc = 2;
    argus[3].argv = (void **)malloc(argus[3].argc * sizeof(void*));
    argus[3].argv[0] = &qVarLock;
    argus[3].argv[1] = &sVarLock;

    for (int i = 0; i < 4; i++) {
        error = pthread_create(&pthreadIDs[i], nullptr, ThreadFunc, &argus[i]);
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
