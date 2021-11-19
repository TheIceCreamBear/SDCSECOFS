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
    if(GetExitCodeThread(thread->thread, &thread->returnVal))
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
    #ifdef _WIN32
    CloseHandle(thread->thread);
    free(thread);
    #else
    free(thread->returnVal);
    free(thread);
    #endif
}

CSSem* semCreate(SEM_NAME name, SEM_VALUE maxValue)
{
    #if defined(_WIN32) // windows
    CSSem* sem = (CSSem*)malloc(sizeof(sem));
    sem->sem = CreateSemaphoreA(NULL, maxValue, maxValue, name);
    sem->count = (LPLONG)malloc(sizeof(LONG));
    *sem->count = maxValue;
    #elif defined(__linux__) || defined(__APPLE__)
    sem = sem_open("main", O_CREAT | O_EXCL, 0644, MAX_SEM);
    sem_unlink("main");
    if(sem == SEM_FAILED)
    {
        printf("createSemaphore error: %d. Exiting...\n", errno);
        exit(0);
    }
    semCount = (int*)malloc(sizeof(int));
    *semCount = MAX_SEM;
    #endif
    return sem;
}

//returns 1 if successful, else 0
int semSignal(CSSem* sem)
{
    #if defined(_WIN32) // windows
    ReleaseSemaphore(sem->sem, 1, sem->count);
    *sem->count = *sem->count + 1;
    #elif defined(__linux__) || defined(__APPLE__)
    sem_post(sem);
    *semCount = *semCount + 1;
    #endif
    
    return 1;
}

//returns 1 if available, else 0
int semWait(CSSem* sem)
{
    #if defined(_WIN32) // windows
    if(WaitForSingleObject(sem->sem, 0) == WAIT_OBJECT_0)
    {
        *sem->count = *sem->count - 1;
        return 1;
    }
    else
    {
        return 0;
    }
    printf("decrementSemaphore error %d. Exiting...\n", GetLastError());
    exit(0);
    #elif defined(__linux__) || defined(__APPLE__)
    if(!sem_trywait(sem))
    {
        *semCount = *semCount - 1;
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

//returns 1 if successful, else 0
int semClose(CSSem* sem)
{
    #if defined(_WIN32) // windows
    CloseHandle(sem->sem);
    free(sem);
    #elif defined(__linux__) || defined(__APPLE__)
    sem_close(sem);
    #endif

    return 1;
}