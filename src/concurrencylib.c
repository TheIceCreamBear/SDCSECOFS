#include "concurrencylib.h"

extern CSSem* vcThreadSem;
extern CSSem* vcThreadSemInitial;
extern CSThread* vcThreadList;

void cobeginThread(void** arg)
{
    #if defined(_WIN32) // windows
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)createThread, arg, 0, NULL);
    #elif defined(__APPLE__) || defined(__linux__)
    pthread_create(NULL, NULL, func, arg);
    #endif
}

CSThread* createThread(void** arg) {
    CSThread* thread = malloc(sizeof(CSThread));
    thread->next = NULL;
    if (thread == NULL) {
        return NULL;
    }
    semWait(vcThreadSemInitial);
    #if defined(_WIN32) // windows
    thread->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)arg[0], arg[1], 0, &(thread->id));
    #elif defined(__APPLE__) || defined(__linux__)
    pthread_create(&thread->thread, NULL, func, arg);
    #endif
    vcThreadList->next = thread;
    vcThreadList = thread;
    semSignal(vcThreadSemInitial);
    free(arg);
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
    if(name == NULL)
    {
        return NULL;
    }
    CSSem* sem = (CSSem*)malloc(sizeof(CSSem));
    sem->next = NULL;
    #if defined(_WIN32) // windows
    sem->sem = CreateSemaphoreA(NULL, maxValue, maxValue, name);
    if(sem->sem == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
    {
        free(sem);
        return NULL;
    }
    sem->count = maxValue;
    #elif defined(__linux__) || defined(__APPLE__)
    if(name == NULL)
    {
        name = "NULL";
    }
    sem->sem = sem_open(name, O_CREAT | O_EXCL, 0644, maxValue);
    sem_unlink("main");
    if(sem->sem == SEM_FAILED)
    {
        printf("createSemaphore error: %d. Exiting...\n", errno);
        exit(0);
    }
    sem->count = maxValue;
    #endif
    if(vcThreadSem != NULL)
    {
        vcThreadSem->next = sem;
        vcThreadSem = sem;
    }
    return sem;
}

//returns 1 if successful, else 0
int semSignal(CSSem* sem)
{
    #if defined(_WIN32) // windows
    ReleaseSemaphore(sem->sem, 1, NULL);
    sem->count = sem->count + 1;
    #elif defined(__linux__) || defined(__APPLE__)
    sem_post(sem->sem);
    sem->count = sem->count + 1;
    #endif
    
    return 1;
}

//returns 1 if available, else 0
int semWait(CSSem* sem)
{
    #if defined(_WIN32) // windows
    if(WaitForSingleObject(sem->sem, 0) == WAIT_OBJECT_0)
    {
        sem->count = sem->count - 1;
        return 1;
    }
    else
    {
        return 0;
    }
    printf("decrementSemaphore error %d. Exiting...\n", GetLastError());
    exit(0);
    #elif defined(__linux__) || defined(__APPLE__)
    if(!sem_trywait(sem->sem))
    {
        sem->count = sem->count - 1;
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
    sem_close(sem->sem);
    free(sem);
    #endif

    return 1;
}