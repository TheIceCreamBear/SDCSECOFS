#include "mutexes.h"

//Create a mutex lock
CSMutex* mutexCreate(char* name)
{
    if(name == NULL)
    {
        return NULL;
    }
    CSMutex* mutex = (CSMutex*)malloc(sizeof(CSMutex));
    if(mutex == NULL) {
        vizconError("vcMutexCreate", 502);
    }
    mutex->next = NULL;
    mutex->name = name;
    mutex->num = -1;
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
        vizconError("vcMutexCreate", 502);
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