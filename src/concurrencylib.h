#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32 // windows
#include <windows.h>
typedef DWORD (*threadFunc) (LPVOID param);
#define THREAD_FUNC_RET DWORD WINAPI
#define THREAD_RET DWORD
#define THREAD_PARAM LPVOID
#elif __linux__ // linux stuff
#include <pthread.h>
typedef void* (*threadFunc) (void* param);
#define THREAD_FUNC_RET void*
#define THREAD_RET void*
#define THREAD_PARAM void*
#elif __APPLE__
#include <pthread.h>
typedef void* (*threadFunc) (void* param);
#define THREAD_FUNC_RET void*
#define THREAD_RET void*
#define THREAD_PARAM void*
#endif

// meta: proto type for the compiled name of the user function so we have access to it in our program
int userMain(void);

// concurrency simulator thread
typedef struct CSThread {
    #if defined(_WIN32) // windows
    HANDLE thread;
    #elif defined(__APPLE__) || defined(__linux__)
    pthread_t thread;
    #endif
    THREAD_RET returnVal;
} CSThread;


CSThread* createThread(threadFunc func, void* arg);

int joinThread(CSThread* thread);

void freeCSThread(CSThread* thread);
