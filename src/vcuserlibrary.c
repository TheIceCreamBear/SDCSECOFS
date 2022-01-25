#include "vcuserlibrary.h"

//Create a thread instance with arguments
//Threads do not begin until vcWaitForCompletion or vcWaitForReturn is called
void vcThreadQueue(threadFunc func, void* arg)
{
    void** arr = (void**)malloc(sizeof(void*)*2);
    arr[0] = func;
    arr[1] = arg;
    CSThread* thread = threadCreate((void*)arr);
    free(arr);
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
    return;
}

//Start all threads created by vcCobegin
void vcThreadStart()
{
    if(vizconThreadListInitial == NULL)
    {
        return;
    }

    //Begin all threads
    vizconThreadList = vizconThreadListInitial;
    while(vizconThreadList != NULL)
    {
        threadStart(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
    }

    //Wait for all threads to complete
    vizconThreadList = vizconThreadListInitial;
    while(vizconThreadList != NULL)
    {
        threadJoin(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
    }

    vizconFree();

    return;
}

//Start all threads created by vcCobegin and return their results
//Results are stored in a void double pointer
THREAD_RET* vcThreadReturn()
{
    if(vizconThreadListInitial == NULL)
    {
        return NULL;
    }
    int i = 0;
    
    //Begin all threads and get number of threads
    vizconThreadList = vizconThreadListInitial;
    while(vizconThreadList != NULL)
    {
        threadStart(vizconThreadList);
        vizconThreadList = vizconThreadList->next;
        i++;
    }
    THREAD_RET* arr = (THREAD_RET*)malloc(sizeof(THREAD_RET)*i);
    i = 0;

    //Wait for all threads to complete and get return values
    vizconThreadList = vizconThreadListInitial;
    while(vizconThreadList != NULL)
    {
        threadJoin(vizconThreadList);
        arr[i++] = vizconThreadList->returnVal;
        vizconThreadList = vizconThreadList->next;
    }

    vizconFree();

    return arr;
}

//Puts the current thread to sleep for specified amount of time in milliseconds
void vcThreadSleep(int milliseconds)
{
    #ifdef _WIN32
    Sleep(milliseconds);
    #else
    usleep(milliseconds*1000);
    #endif
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
    vcThreadSleep(20);
}

//Consume one permit from a sempahore, or return immediately if none are available
//Returns 1 if successful, else 0
int vcSemTryWait(vcSem* sem)
{
    if(semTryWait(sem))
    {
        return 1;
    }
    return 0;
}

//Release one permit from a semaphore
void vcSemSignal(vcSem* sem)
{
    semSignal(sem);
    vcThreadSleep(20);
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
}

int vcMutexTrylock(vcMutex* mutex)
{
    if(mutexTryLock(mutex))
    {
        return 1;
    }
    return 0;
}

void vcMutexUnlock(vcMutex* mutex)
{
    mutexUnlock(mutex);
}

int vcMutexStatus(vcMutex* mutex)
{
    return mutexStatus(mutex);
}