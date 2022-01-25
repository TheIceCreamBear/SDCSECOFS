#include "concurrencylib.h"

//Create a CSThread that does not start until WaitForCompletion or WaitForReturn is called
//arg parameter must have funcion at head of array
CSThread* threadCreate(void* arg) 
{
    CSThread* thread = (CSThread*)malloc(sizeof(CSThread));
    if (thread == NULL) 
    {
        vizconError("vcThreadAdd", 8);
    }
    thread->next = NULL;
    void** arr = (void**)arg;
    #if defined(_WIN32) // windows
    thread->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)arr[0], arr[1], CREATE_SUSPENDED, &(thread->id));
    if(thread->thread == NULL)
    {
        int err = (int)GetLastError();
        free(thread);
        free(arg);
        vizconError("vcThreadAdd", err);
    }
    #elif defined(__APPLE__) || defined(__linux__)
    int err = pthread_create(&thread->thread, NULL, arr[0], arr[1]);
    if(err)
    {
        free(thread);
        vizconError("vcThreadAdd", err);
    }
    #endif
    return thread;
}

//Start a given thread that was in a suspended state
void threadStart(CSThread* thread)
{
    #if defined(_WIN32) // windows
    ResumeThread(thread->thread);
    #elif defined(__APPLE__) || defined(__linux__)
    printf("lol");
    #endif
}

 //Waits for thread to complete before being joined back into main function
void threadJoin(CSThread* thread) 
{
    #if defined(_WIN32) // windows
    DWORD ret = WaitForSingleObject(thread->thread, INFINITE);
    if(ret == WAIT_FAILED)
    {
        vizconError("vcThreadStart/vcThreadReturn", GetLastError());
    }
    else if(ret == WAIT_OBJECT_0)
    {
        if(!GetExitCodeThread(thread->thread, &thread->returnVal))
        {
            vizconError("vcThreadStart/vcThreadReturn", GetLastError());
        }
    }
    else if(ret == WAIT_ABANDONED)
    {
        vizconError("vcThreadStart/vcThreadReturn", 500);
    }
    else if (ret == WAIT_TIMEOUT)
    {
        vizconError("vcThreadStart/vcThreadReturn", 501);
    }
    #elif defined(__APPLE__) || defined(__linux__)
    int err = pthread_join(thread->thread, &thread->returnVal);
    if(err)
    {
        free(thread);
        vizconError("vcThreadStart/vcThreadReturn", err);
    }
    #endif
}

//Frees all data associated with a CSThread type, including itself
void threadClose(CSThread* thread) 
{
    #ifdef _WIN32
    if(!CloseHandle(thread->thread))
    {
        vizconError("vcThreadStart/vcThreadReturn", GetLastError());
    }
    free(thread);
    #else
    free(thread);
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
        vizconError("vcSemCreate", 8);
    }
    sem->next = NULL;
    #if defined(_WIN32) // windows
    sem->sem = CreateSemaphoreA(NULL, maxValue, maxValue, name);
    if(sem->sem == NULL)
    {
        int err = (int)GetLastError();
        free(sem);
        vizconError("vcSemCreate", err);
    }
    sem->count = maxValue;
    #elif defined(__linux__) || defined(__APPLE__)
    sem->sem = sem_open(name, O_CREAT | O_EXCL, 0644, maxValue);
    if(sem->sem == SEM_FAILED)
    {
        free(sem);
        vizconError("vcSemCreate", errno);
    }
    if(sem_unlink(name))
    {
        free(sem);
        vizconError("vcSemCreate", errno);
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
        vizconError("vcSemSignal", GetLastError());
    }
    sem->count = sem->count + 1;
    #elif defined(__linux__) || defined(__APPLE__)
    if(sem_post(sem->sem))
    {
        vizconError("vcSemSignal", errno);
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
        vizconError("vcSemWait", GetLastError());
    }
    else if(ret == WAIT_OBJECT_0)
    {
        sem->count = sem->count - 1;
    }
    else if(ret == WAIT_ABANDONED)
    {
        vizconError("vcSemWait", 500);
    }
    else if (ret == WAIT_TIMEOUT)
    {
        vizconError("vcSemWait", 501);
    }
    #elif defined(__linux__) || defined(__APPLE__)
    if(sem_wait(sem->sem))
    {
        vizconError("vcSemWait", errno);
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
        vizconError("vcSemTryWait", GetLastError());
    }
    else if(ret == WAIT_OBJECT_0)
    {
        sem->count = sem->count - 1;
        return 1;
    }
    else if(ret == WAIT_ABANDONED)
    {
        vizconError("vcSemTryWait", 500);
    }
    #elif defined(__linux__) || defined(__APPLE__)
    if(!sem_trywait(sem->sem))
    {
        sem->count = sem->count - 1;
        return 1;
    }
    else if(errno != EAGAIN)
    {
        vizconError("vcSemTryWait", errno);
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
        vizconError("vcSemClose", GetLastError());
    }
    free(sem);
    #elif defined(__linux__) || defined(__APPLE__)
    if(sem_close(sem->sem))
    {
        vizconError("vcSemClose", errno);
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
        vizconError("vcMutexCreate", 8);
    }
    mutex->next = NULL;
    #if defined(_WIN32) // windows
    mutex->mutex = CreateMutexA(NULL, FALSE, name);
    if(mutex->mutex == NULL)
    {
        int err = (int)GetLastError();
        free(mutex);
        vizconError("vcMutexCreate", err);
    }
    #elif defined(__linux__) || defined(__APPLE__)
    mutex->mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if(mutex->mutex == NULL) {
        free(mutex);
        vizconError("vcMutexCreate", 8);
    }
    int err = pthread_mutex_init(mutex->mutex, NULL);
    if(err)
    {
        free(mutex->mutex);
        free(mutex);
        vizconError("vcMutexCreate", errno);
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
        vizconError("vcMutexLock", GetLastError());
    }
    else if(ret == WAIT_ABANDONED)
    {
        vizconError("vcMutexLock", 500);
    }
    else if (ret == WAIT_TIMEOUT)
    {
        vizconError("vcMutexLock", 501);
    }
    #elif defined(__linux__) || defined(__APPLE__)
    int err = pthread_mutex_lock(mutex->mutex);
    if(err)
    {
        vizconError("vcMutexLock", err);
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
        vizconError("vcMutexTryLock", GetLastError());
    }
    else if(ret == WAIT_OBJECT_0)
    {
        return 1;
    }
    else if(ret == WAIT_ABANDONED)
    {
        vizconError("vcMutexTryLock", 500);
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
            vizconError("vcMutexTryLock", err);
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
        vizconError("vcMutexUnlock", GetLastError());
    }
    #elif defined(__linux__) || defined(__APPLE__)
    int err = pthread_mutex_unlock(mutex->mutex);
    if(err) 
    {
        vizconError("vcMutexUnlock", err);
    }
    #endif
}

//Close a mutex lock
void mutexClose(CSMutex* mutex)
{
    #if defined(_WIN32) // windows
    if(!CloseHandle(mutex->mutex))
    {
        vizconError("vcMutexClose", GetLastError());
    }
    free(mutex);
    #elif defined(__linux__) || defined(__APPLE__)
    int err = pthread_mutex_destroy(mutex->mutex);
    if(err) 
    {
        vizconError("vcMutexClose", err);
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
void vizconError(char* func, int err)
{
    vizconAbort();
    printf("\nError from %s\n", func);
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
    printf("thread is %d\n", id);
    while(vizconThreadListInitial != NULL)
    {
        printf("found %d\n", vizconThreadListInitial->id);
        if(vizconThreadListInitial->id != id)
        {
            printf("using %d\n", vizconThreadListInitial->id);
            TerminateThread(vizconThreadListInitial->thread, dwExitCode);
        }
        vizconThreadListInitial = vizconThreadListInitial->next;
    }
    #elif defined(__linux__) || defined(__APPLE__)
    int id = pthread_self();
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
    //free all user threads
    while(vizconThreadListInitial != NULL)
    {
        vizconThreadList = vizconThreadListInitial->next;
        threadClose(vizconThreadListInitial);
        vizconThreadListInitial = vizconThreadList;
    }

    //Free all semaphores
    while(vizconSemListInitial != NULL)
    {
        vizconSemList = vizconSemListInitial->next;
        semClose(vizconSemListInitial);
        vizconSemListInitial = vizconSemList;
    }

    //Free all mutex locks
    while(vizconMutexListInitial != NULL)
    {
        vizconMutexList = vizconMutexListInitial->next;
        mutexClose(vizconMutexListInitial);
        vizconMutexListInitial = vizconMutexList;
    }
}