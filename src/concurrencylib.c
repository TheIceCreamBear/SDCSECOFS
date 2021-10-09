#include "concurrencylib.h"

CSThread* createThread(threadFunc func, void* arg) {
    CSThread* thread = malloc(sizeof(CSThread));
    if (thread == NULL) {
        return NULL;
    }
    #if defined(_WIN32) // windows
    thread->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, &(thread->id));
    #elif defined(__APPLE__) || defined(__linux__)
    pthread_create(&thread->thread, NULL, func, arg);
    #endif

    return thread;
}

int joinThread(CSThread* thread) {
    #if defined(_WIN32) // windows
    WaitForSingleObject(thread->thread, INFINITE);
    if(GetExitCodeThread(thread->thread, &(thread->returnVal)))
    {
        return 0;
    }
    else
    {
        printf("joinThread error: %d. Exiting...\n", GetLastError());
        exit(0);
    }
    #elif defined(__APPLE__) || defined(__linux__)
    pthread_join(thread->thread, &thread->returnVal);
    #endif
    // return 0 if error, this is a TODO
    return 1;
}

void freeCSThread(CSThread* thread) {
    // make sure it is joined
    // TODO

    // free struct
    free(thread);
}

void createSemaphore()
{
    #if defined(_WIN32) // windows
    sem = CreateSemaphore(NULL, MAX_SEM, MAX_SEM, NULL);
    #elif defined(__APPLE__) || defined(__linux__)
    if(sem_init(sem, 0, MAX_SEM))
    {
        printf("createSemaphore error: %d. Exiting...\n", errno);
        exit(0);
    }
    #endif
    
    return;
}