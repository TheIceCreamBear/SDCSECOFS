#include "vcuserlibrary.h"

//Create a thread instance with arguments
//Threads do not begin until vcWaitForCompletion or vcWaitForReturn is called
void vcCobegin(threadFunc func, void* arg)
{
    if(vizconThreadSem == NULL)
    {
        vizconThreadSem = semCreate("/vizconThreadSem", 1);
        semWait(vizconThreadSem);
        vizconThreadSem->next = semCreate("/vizconAbortSem", 1);
    }
    void** arr = (void**)malloc(sizeof(void*)*2);
    arr[0] = func;
    arr[1] = arg;
    void* data = arr;
    CSThread* thread = cobeginThread(data);
    if(vizconCobeginList == NULL)
    {
        vizconCobeginList = thread;
        vizconCobeginListInitial = thread;
    }
    else
    {
        vizconCobeginList->next = thread;
        vizconCobeginList = thread;
    }
    vizconThreadSem->count = vizconThreadSem->count + 1;
    return;
}

//Start all threads created by vcCobegin
void vcWaitForCompletion()
{
    if(vizconCobeginListInitial == NULL)
    {
        return;
    }
    CSThread* iterList = vizconCobeginListInitial;

    //Release all thread creators and ensure they have all been joined
    semSignal(vizconThreadSem);
    while(iterList != NULL)
    {
        vizconCobeginList = iterList->next;
        joinThread(iterList);
        iterList = vizconCobeginList;
    }

    //Begin to join all user threads, as well as populate return array
    iterList = vizconThreadListInitial;
    while(iterList != NULL)
    {
        vizconThreadList = iterList->next;
        joinThread(iterList);
        iterList = vizconThreadList;
    }

    vizconFree();

    return;
}

//Start all threads created by vcCobegin and return their results
//Results are stored in a void double pointer
THREAD_RET* vcWaitForReturn()
{
    if(vizconCobeginListInitial == NULL)
    {
        return NULL;
    }
    int i = 0;
    THREAD_RET* arr = (THREAD_RET*)malloc(sizeof(THREAD_RET)*vizconThreadSem->count);
    CSThread* iterList = vizconCobeginListInitial;

    //Release all thread creators and ensure they have all been joined
    semSignal(vizconThreadSem);
    while(iterList != NULL)
    {
        vizconCobeginList = iterList->next;
        joinThread(iterList);
        iterList = vizconCobeginList;
    }

    //Begin to join all user threads, as well as populate return array
    iterList = vizconThreadListInitial;
    while(iterList != NULL)
    {
        vizconThreadList = iterList->next;
        joinThread(iterList);
        arr[i++] = iterList->returnVal;
        iterList = vizconThreadList;
    }

    vizconFree();

    return arr;
}

//Create a semaphore with a name and maximum permit count
//All semaphores must have a name, and values must be an integer greater than zero
vcSem* vcSemCreate(char* name, int count)
{
    vcSem* sem = semCreate(name, count);
    if(vizconSemList == NULL)
    {
        vizconSemListInitial = sem;
        vizconSemList = sem;
    }
    else
    {
        vizconSemList->next = sem;
        vizconSemList = sem;
    }
    return sem;
}

//Consume one permit from a semaphore, or wait until one is available.
void vcSemWait(vcSem* sem)
{
    semWait(sem);
    sleepThread(20);
}

//Consume one permit from a sempahore, or return immediately if none are available
//Returns 1 if successful, else 0
int vcSemTryWait(vcSem* sem)
{
    if(semTryWait(sem))
    {
        sleepThread(20);
        return 1;
    }
    sleepThread(20);
    return 0;
}

//Release one permit from a semaphore
void vcSemSignal(vcSem* sem)
{
    semSignal(sem);
    sleepThread(20);
}

//Return the current number of permits from semaphore
int vcSemValue(vcSem* sem)
{
    return semValue(sem);
}

vcMutex* vcMutexCreate(char* name)
{
    vcMutex* mutex = mutexCreate(name);
    if(vizconMutexList == NULL)
    {
        vizconMutexListInitial = mutex;
        vizconMutexList = mutex;
    }
    else
    {
        vizconMutexList->next = mutex;
        vizconMutexList = mutex;
    }
    return mutex;
}

void vcMutexLock(vcMutex* mutex)
{
    mutexLock(mutex);
    sleepThread(20);
}

int vcMutexTrylock(vcMutex* mutex)
{
    if(mutexTryLock(mutex))
    {
        sleepThread(20);
        return 1;
    }
    sleepThread(20);
    return 0;
}

void vcMutexUnlock(vcMutex* mutex)
{
    mutexUnlock(mutex);
    sleepThread(20);
}

int vcMutexStatus(vcMutex* mutex)
{
    return mutexStatus(mutex);
}