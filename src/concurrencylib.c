#include "concurrencylib.h"

extern CSThread* vizconThreadList;
extern CSThread* vizconThreadListInitial;
extern CSSem* vizconThreadSem;

//Create a thread instance of the createThread function
CSThread* cobeginThread(void* arg)
{
    CSThread* thread = malloc(sizeof(CSThread));
    thread->next = NULL;
    #if defined(_WIN32) // windows
    thread->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)createThread, arg, 0, &(thread->id));
    #elif defined(__APPLE__) || defined(__linux__)
    pthread_create(&thread->thread, NULL, createThread, arg);
    #endif
    return thread;
}

//Create a CSThread that does not start until WaitForCompletion or WaitForReturn is called
//arg parameter must have funcion at head of array
THREAD_RET createThread(void* arg) {
    CSThread* thread = malloc(sizeof(CSThread));
    thread->next = NULL;
    if (thread == NULL) {
        return (THREAD_RET)-1;
    }
    void** arr = (void**)arg;
    semWait(vizconThreadSem);
    #if defined(_WIN32) // windows
    thread->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)arr[0], arr[1], 0, &(thread->id));
    #elif defined(__APPLE__) || defined(__linux__)
    pthread_create(&thread->thread, NULL, arr[0], arr[1]);
    #endif
    if(vizconThreadListInitial == NULL)
    {
        vizconThreadList = thread;
        vizconThreadListInitial = thread;
    }
    else
    {
        vizconThreadList->next = thread;
        vizconThreadList = thread;
    }
    semSignal(vizconThreadSem);
    free(arg);
}
 //Waits for thread to complete before being joined back into main function
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

//Frees all data associated with a CSThread type, including itself
void freeCSThread(CSThread* thread) {
    // make sure it is joined
    // TODO
    // free struct
    #ifdef _WIN32
    CloseHandle(thread->thread);
    free(thread);
    #else
    free(thread);
    #endif
}

//Puts the current thread to sleep for specified amount of time in milliseconds
void sleepThread(int milliseconds)
{
    #ifdef _WIN32
    Sleep(milliseconds);
    #else
    usleep(milliseconds*1000);
    #endif
}

//Create a CSSem
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
    sem->sem = sem_open(name, O_CREAT | O_EXCL, 0644, maxValue);
    sem_unlink(name);
    if(sem->sem == SEM_FAILED)
    {
        printf("createSemaphore error: %d. Exiting...\n", errno);
        exit(0);
    }
    sem->count = maxValue;
    #endif
    return sem;
}

//Releases 1 permit from semaphore and increment its count
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

//Waits for semaphore to become available, attaining 1 permit from semaphore and decrementing its count
//returns 1 if available, else 0
int semWait(CSSem* sem)
{
    #if defined(_WIN32) // windows
    if(WaitForSingleObject(sem->sem, INFINITE) == WAIT_OBJECT_0)
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
    if(!sem_wait(sem->sem))
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

//Try to attain 1 permit from semaphore and decrement its count
//Returns immediately if semaphore has no remaining permits at time of call
//returns 1 if available, else 0
int semTryWait(CSSem* sem)
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

//Returns semaphore's current value
int semValue(CSSem* sem)
{
    return sem->count;
}

//Frees all data associated with a CSSem type, including itself
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