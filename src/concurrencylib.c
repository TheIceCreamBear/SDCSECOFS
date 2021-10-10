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
    CloseHandle(thread);
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

void incrementSemaphore()
{
    #if defined(_WIN32) // windows
    ReleaseSemaphore(sem, 1, NULL);
    #elif defined(__APPLE__) || defined(__linux__)
    sem_post(sem);
    #endif
    
    return;
}

//returns 1 if available, else 0
int decrementSemaphore()
{
    #if defined(_WIN32) // windows
    if(WaitForSingleObject(sem, 0) == WAIT_OBJECT_0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    printf("decrementSemaphore error %d. Exiting...\n", GetLastError());
    exit(0);
    #elif defined(__APPLE__) || defined(__linux__)
    if(!sem_wait(sem))
    {
        return 1;
    }
    else
    {
        return 0;
    }
    printf("decrementSemaphore error %d. Exiting...\n", errno);
    exit(0);
    #endif
    
    return 0;
}

void freeSemaphore()
{
    #if defined(_WIN32) // windows
    CloseHandle(sem);
    #elif defined(__APPLE__) || defined(__linux__)
    sem_close(sem);
    #endif
    
    return;
}