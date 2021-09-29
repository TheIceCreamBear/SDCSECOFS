#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32 // windows
#include <windows.h>
typedef DWORD WINAPI (*threadFunc) (LPVOID param);
#define THREAD_RET DWORD WINAPI
#define THREAD_PARAM LPVOID
#elif __linux__ // linux stuff
#include <pthread.h>
typedef void* (*threadFunc) (void* param);
#define THREAD_RET void*
#define THREAD_PARAM void*
#elif __APPLE__
#include <pthread.h>
typedef void* (*threadFunc) (void* param);
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
    void* returnVal;
} CSThread;


CSThread* createThread(threadFunc func, void* arg) {
    CSThread* thread = malloc(sizeof(CSThread));
    if (thread == NULL) {
        return NULL;
    }

    #if defined(_WIN32) // windows
    thread->thread = CreateThread(0, 0, func, arg, 0, 0);
    #elif defined(__APPLE__) || defined(__linux__)
    pthread_create(&thread->thread, NULL, func, arg);
    #endif

    return thread;
}

int joinThread(CSThread* thread) {
    #if defined(_WIN32) // windows
    WaitForSingleObject(thread->thread, INFINITE);
    GetExitCodeThread(thread->thread, &((DWORD) thread->returnVal));
    #elif defined(__APPLE__) || defined(__linux__)
    pthread_join(thread->thread, &thread->returnVal);
    #endif
}

void freeCSThread(CSThread* thread) {
    // make sure it is joined
    // TODO

    // free struct
    free(thread);
}
