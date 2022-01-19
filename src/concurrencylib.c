#include "concurrencylib.h"

//Create a thread instance of the createThread function
CSThread* cobeginThread(void* arg)
{
    CSThread* thread = (CSThread*)malloc(sizeof(CSThread));
    if(thread == NULL) 
    {
        vizconError(0, 8);
    }
    thread->next = NULL;
    #if defined(_WIN32) // windows
    thread->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)createThread, arg, 0, &(thread->id));
    if(thread->thread == NULL)
    {
        int err = (int)GetLastError();
        free(thread);
        vizconError(0, err);
    }
    #elif defined(__APPLE__) || defined(__linux__)
    int err = pthread_create(&thread->thread, NULL, createThread, arg);
    if(err)
    {
        free(thread);
        vizconError(0, err);
    }
    #endif
    return thread;
}

//Create a CSThread that does not start until WaitForCompletion or WaitForReturn is called
//arg parameter must have funcion at head of array
THREAD_RET createThread(void* arg) {
    CSThread* thread = (CSThread*)malloc(sizeof(CSThread));
    if (thread == NULL) 
    {
        vizconError(0, 8);
    }
    thread->next = NULL;
    void** arr = (void**)arg;
    semWait(vizconThreadSem);
    #if defined(_WIN32) // windows
    thread->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)arr[0], arr[1], 0, &(thread->id));
    if(thread->thread == NULL)
    {
        int err = (int)GetLastError();
        free(thread);
        vizconError(0, err);
    }
    #elif defined(__APPLE__) || defined(__linux__)
    int err = pthread_create(&thread->thread, NULL, arr[0], arr[1]);
    if(err)
    {
        free(thread);
        vizconError(0, err);
    }
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
    return (THREAD_RET)1;
}

 //Waits for thread to complete before being joined back into main function
void joinThread(CSThread* thread) {
    #if defined(_WIN32) // windows
    DWORD ret = WaitForSingleObject(thread->thread, INFINITE);
    if(ret == WAIT_FAILED)
    {
        vizconError(1, GetLastError());
    }
    else if(ret == WAIT_OBJECT_0)
    {
        if(!GetExitCodeThread(thread->thread, &thread->returnVal))
        {
            vizconError(1, GetLastError());
        }
    }
    else if(ret == WAIT_ABANDONED)
    {
        vizconError(1, 500);
    }
    else if (ret == WAIT_TIMEOUT)
    {
        vizconError(1, 501);
    }
    #elif defined(__APPLE__) || defined(__linux__)
    int err = pthread_join(thread->thread, &thread->returnVal);
    if(err)
    {
        free(thread);
        vizconError(1, err);
    }
    #endif
}

//Frees all data associated with a CSThread type, including itself
void freeCSThread(CSThread* thread) {
    #ifdef _WIN32
    if(!CloseHandle(thread->thread))
    {
        vizconError(1, GetLastError());
    }
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
    if (sem == NULL) {
        vizconError(3, 8);
    }
    sem->next = NULL;
    #if defined(_WIN32) // windows
    sem->sem = CreateSemaphoreA(NULL, maxValue, maxValue, name);
    if(sem->sem == NULL)
    {
        int err = (int)GetLastError();
        free(sem);
        vizconError(3, err);
    }
    sem->count = maxValue;
    #elif defined(__linux__) || defined(__APPLE__)
    sem->sem = sem_open(name, O_CREAT | O_EXCL, 0644, maxValue);
    if(sem->sem == SEM_FAILED)
    {
        free(sem);
        vizconError(3, errno);
    }
    if(sem_unlink(name))
    {
        free(sem);
        vizconError(3, errno);
    }
    sem->count = maxValue;
    #endif
    return sem;
}

//Releases 1 permit from semaphore and increment its count
void semSignal(CSSem* sem)
{
    #if defined(_WIN32) // windows
    if(!ReleaseSemaphore(sem->sem, 1, NULL))
    {
        vizconError(6, GetLastError());
    }
    sem->count = sem->count + 1;
    #elif defined(__linux__) || defined(__APPLE__)
    if(sem_post(sem->sem))
    {
        vizconError(6, errno);
    }
    sem->count = sem->count + 1;
    #endif
}

//Waits for semaphore to become available, attaining 1 permit from semaphore and decrementing its count
void semWait(CSSem* sem)
{
    #if defined(_WIN32) // windows
    DWORD ret = WaitForSingleObject(sem->sem, INFINITE);
    if(ret == WAIT_FAILED)
    {
        vizconError(4, GetLastError());
    }
    else if(ret == WAIT_OBJECT_0)
    {
        sem->count = sem->count - 1;
    }
    else if(ret == WAIT_ABANDONED)
    {
        vizconError(4, 500);
    }
    else if (ret == WAIT_TIMEOUT)
    {
        vizconError(4, 501);
    }
    #elif defined(__linux__) || defined(__APPLE__)
    if(sem_wait(sem->sem))
    {
        vizconError(4, errno);
    }
    #endif
}

//Try to attain 1 permit from semaphore and decrement its count
//Returns immediately if semaphore has no remaining permits at time of call
//returns 1 if available, else 0
int semTryWait(CSSem* sem)
{
    #if defined(_WIN32) // windows
    DWORD ret = WaitForSingleObject(sem->sem, 0);
    if(ret == WAIT_FAILED)
    {
        vizconError(5, GetLastError());
    }
    else if(ret == WAIT_OBJECT_0)
    {
        sem->count = sem->count - 1;
        return 1;
    }
    else if(ret == WAIT_ABANDONED)
    {
        vizconError(5, 500);
    }
    #elif defined(__linux__) || defined(__APPLE__)
    if(!sem_trywait(sem->sem))
    {
        sem->count = sem->count - 1;
        return 1;
    }
    else if(errno != EAGAIN)
    {
        vizconError(5, errno);
    }
    #endif
    return 0;
}

//Returns semaphore's current value
int semValue(CSSem* sem)
{
    return sem->count;
}

//Frees all data associated with a CSSem type, including itself
void semClose(CSSem* sem)
{
    #if defined(_WIN32) // windows
    if(!CloseHandle(sem->sem))
    {
        vizconError(1, GetLastError());
    }
    free(sem);
    #elif defined(__linux__) || defined(__APPLE__)
    if(sem_close(sem->sem))
    {
        vizconError(1, errno);
    }
    free(sem);
    #endif
}

//Create a mutex lock
CSMutex* mutexCreate(char* name)
{
    if(name == NULL)
    {
        return NULL;
    }
    CSMutex* mutex = (CSMutex*)malloc(sizeof(CSMutex));
    if(mutex == NULL) {
        vizconError(8, 8);
    }
    mutex->next = NULL;
    #if defined(_WIN32) // windows
    mutex->mutex = CreateMutexA(NULL, FALSE, name);
    if(mutex->mutex == NULL)
    {
        int err = (int)GetLastError();
        free(mutex);
        vizconError(8, err);
    }
    #elif defined(__linux__) || defined(__APPLE__)
    mutex->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if(mutex->mutex == NULL) {
        free(mutex);
        vizconError(8, 8);
    }
    int err = pthread_mutex_init(mutex->mutex, NULL);
    if(err)
    {
        free(mutex->mutex);
        free(mutex);
        vizconError(8, errno);
    }
    #endif
    return mutex;
}

//Attain a mutex lock, or wait until it becomes available
void mutexLock(CSMutex* mutex)
{
    #if defined(_WIN32) // windows
    DWORD ret = WaitForSingleObject(mutex->mutex, INFINITE);
    if(ret == WAIT_FAILED)
    {
        vizconError(9, GetLastError());
    }
    else if(ret == WAIT_ABANDONED)
    {
        vizconError(9, 500);
    }
    else if (ret == WAIT_TIMEOUT)
    {
        vizconError(9, 501);
    }
    #elif defined(__linux__) || defined(__APPLE__)
    int err = pthread_mutex_lock(mutex->mutex);
    if(err)
    {
        vizconError(9, err);
    }
    #endif
}

//Try to obtain a mutex lock
//Returns immediately if the mutex lock is unavailable
//returns 1 if available, else 0
int mutexTryLock(CSMutex* mutex)
{
    #if defined(_WIN32) // windows
    DWORD ret = WaitForSingleObject(mutex->mutex, 0);
    if(ret == WAIT_FAILED)
    {
        vizconError(10, GetLastError());
    }
    else if(ret == WAIT_OBJECT_0)
    {
        return 1;
    }
    else if(ret == WAIT_ABANDONED)
    {
        vizconError(10, 500);
    }
    #elif defined(__linux__) || defined(__APPLE__)
    int err = pthread_mutex_trylock(mutex->mutex);
    if(!err)
    {
        return 1;
    }
    else
    {
        if(err != EBUSY)
        {
            vizconError(10, err);
        }
    }
    #endif
    return 0;
}

//Release a mutex lock
void mutexUnlock(CSMutex* mutex)
{
    #if defined(_WIN32) // windows
    if(!ReleaseMutex(mutex->mutex))
    {
        vizconError(11, GetLastError());
    }
    #elif defined(__linux__) || defined(__APPLE__)
    int err = pthread_mutex_unlock(mutex->mutex);
    if(err) 
    {
        vizconError(11, err);
    }
    #endif
}

//Close a mutex lock
void mutexClose(CSMutex* mutex)
{
    #if defined(_WIN32) // windows
    if(!CloseHandle(mutex->mutex))
    {
        vizconError(1, GetLastError());
    }
    free(mutex);
    #elif defined(__linux__) || defined(__APPLE__)
    int err = pthread_mutex_destroy(mutex->mutex);
    if(err) 
    {
        vizconError(1, err);
    }
    free(mutex->mutex);
    free(mutex);
    #endif
}

//Check if a mutex lock is available
//Returns 1 if true, else 0
int mutexStatus(CSMutex* mutex)
{
    #if defined(_WIN32) // windows
    if(mutexTryLock(mutex))
    {
        mutexUnlock(mutex);
        return 1;
    }
    #elif defined(__linux__) || defined(__APPLE__)
    if(pthread_mutex_trylock(mutex->mutex))
    {
        mutexUnlock(mutex);
        return 1;
    }
    #endif
    return 0;
}

//Handles error from concurrencylib and vcuserlibrary
void vizconError(int func, int err)
{
    semWait(vizconThreadSem->next);
    vizconAbort();
    printf("\nError from ");
    switch(func)
    {
        case 0:
        {
            printf("vcCobegin\n");
            break;
        }
        case 1:
        {
            printf("vcWaitForCompletion\n");
            break;
        }
        case 2:
        {
            printf("vcWaitForReturn\n");
            break;
        }
        case 3:
        {
            printf("vcSemCreate\n");
            break;
        }
        case 4:
        {
            printf("vcSemWait\n");
            break;
        }
        case 5:
        {
            printf("vcSemTryWait\n");
            break;
        }
        case 6:
        {
            printf("vcSemSignal\n");
            break;
        }
        case 7:
        {
            printf("vcSemValue\n");
            break;
        }
        case 8:
        {
            printf("vcMutexCreate\n");
            break;
        }
        case 9:
        {
            printf("vcMutexLock\n");
            break;
        }
        case 10:
        {
            printf("vcMutexTryLock\n");
            break;
        }
        case 11:
        {
            printf("vcMutexUnlock\n");
            break;
        }
        case 12:
        {
            printf("vcMutexStatus\n");
            break;
        }
        default:
        {
            printf("unknown function\n");
        }
    }
    #if defined(_WIN32) // windows
    LPSTR message;
    if(err < 500)
    {
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message, 0, NULL);
        printf("system error ");
    }
    #elif defined(__linux__) || defined(__APPLE__)
    char* message;
    if(err < 500)
    {
        message = strerror(err);
        printf("errno ");
    }
    #endif
    if(err >= 500)
    {
        printf("vizcon error ");
        switch(err)
        {
            case 500:
            {
                message = "A thread terminated without releasing its mutex lock.";
                break;
            }
            case 501:
            {
                message = "An unexpected wait timeout occurred.";
                break;
            }
            default:
            {
                message = "An unknown error has occurred.";
            }
        }
    }
    printf("code %d: %s\n", err, message);
    exit(0);
}

//Kill all threads and free all concurrency structures
void vizconAbort()
{
    #if defined(_WIN32) // windows
    DWORD dwExitCode = 0;
    DWORD id = GetCurrentThreadId();
    while(vizconCobeginListInitial != NULL)
    {
        if(vizconCobeginListInitial->id != id)
        {
            TerminateThread(vizconCobeginListInitial->thread, dwExitCode);
        }
        vizconCobeginListInitial = vizconCobeginListInitial->next;
    }
    while(vizconThreadListInitial != NULL)
    {
        if(vizconThreadListInitial->id != id)
        {
            TerminateThread(vizconThreadListInitial->thread, dwExitCode);
        }
        vizconThreadListInitial = vizconThreadListInitial->next;
    }
    #elif defined(__linux__) || defined(__APPLE__)
    int id = pthread_self();
    while(vizconCobeginListInitial != NULL)
    {
        if((int)vizconCobeginListInitial->thread != id)
        {
            pthread_cancel(vizconCobeginListInitial->thread);
        }
        vizconCobeginListInitial = vizconCobeginListInitial->next;
    }
    while(vizconThreadListInitial != NULL)
    {
        if((int)vizconThreadListInitial->thread != id)
        {
            pthread_cancel(vizconThreadListInitial->thread);
        }
        vizconThreadListInitial = vizconThreadListInitial->next;
    }
    #endif

    vizconFree();
}

//Free all vizcon data
void vizconFree()
{
    //free all cobegin thread
    while(vizconCobeginListInitial != NULL)
    {
        vizconCobeginList = vizconCobeginListInitial->next;
        freeCSThread(vizconCobeginListInitial);
        vizconCobeginListInitial = vizconCobeginList;
    }

    //free all user threads
    while(vizconThreadListInitial != NULL)
    {
        vizconThreadList = vizconThreadListInitial->next;
        freeCSThread(vizconThreadListInitial);
        vizconThreadListInitial = vizconThreadList;
    }

    //Free all semaphores
    while(vizconSemListInitial != NULL)
    {
        vizconSemList = vizconSemListInitial->next;
        semClose(vizconSemListInitial);
        vizconSemListInitial = vizconSemList;
    }
    semClose(vizconThreadSem->next);
    semClose(vizconThreadSem);

    //Free all mutex locks
    while(vizconMutexListInitial != NULL)
    {
        vizconMutexList = vizconMutexListInitial->next;
        mutexClose(vizconMutexListInitial);
        vizconMutexListInitial = vizconMutexList;
    }
}