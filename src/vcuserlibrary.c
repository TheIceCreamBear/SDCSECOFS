#include "vcuserlibrary.h"

//Create a thread instance with arguments
//Threads do not begin until vcWaitForCompletion or vcWaitForReturn is called
void vcThreadQueue(threadFunc func, void* arg)
{
    CSThread* thread = threadCreate(func, arg);
    if(vizconThreadListInitial == NULL)
    {
        thread->name = vizconCreateName(0, 0);
        thread->num = 0;
        vizconThreadList = thread;
        vizconThreadListInitial = thread;
    }
    else
    {
        thread->name = vizconCreateName(0, vizconThreadList->num + 1);
        thread->num = vizconThreadList->num + 1;
        vizconThreadList->next = thread;
        vizconThreadList = thread;
    }
    return;
}

void vcThreadQueueNamed(threadFunc func, void* arg, char* name)
{
    CSThread* thread = threadCreate(func, arg);
    char* mallocName = (char*)malloc(sizeof(char) * (vizconStringLength(name) + 1));
    if (mallocName == NULL) 
    {
        vizconError("vcThreadQueueNamed", 502);
    }
    sprintf(mallocName, "%s", name);
    thread->name = mallocName;
    if(vizconThreadListInitial == NULL)
    {
        thread->num = 0;
        vizconThreadList = thread;
        vizconThreadListInitial = thread;
    }
    else
    {
        thread->num = vizconThreadList->num + 1;
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

//todo
vcSem* vcSemCreate(int count)
{
    vcSem* sem;
    if(vizconSemList == NULL)
    {
        sem = semCreate(vizconCreateName(1, 0), count);
        sem->num = 0;
        vizconSemListInitial = sem;
        vizconSemList = sem;
    }
    else
    {
        sem = semCreate(vizconCreateName(1, vizconSemList->num + 1), count);
        sem->num = vizconSemList->num + 1;
        vizconSemList->next = sem;
        vizconSemList = sem;
    }
    return sem;
}

//Create a semaphore with a name and maximum permit count
//All semaphores must have a name, and values must be an integer greater than zero
vcSem* vcSemCreateNamed(int count, char* name)
{
    char* mallocName = (char*)malloc(sizeof(char) * (vizconStringLength(name) + 1));
    if (mallocName == NULL) 
    {
        vizconError("vcSemCreateNamed", 502);
    }
    sprintf(mallocName, "%s", name);
    vcSem* sem = semCreate(mallocName, count);
    if(vizconSemList == NULL)
    {
        sem->num = 0;
        vizconSemListInitial = sem;
        vizconSemList = sem;
    }
    else
    {
        sem->num = vizconSemList->num + 1;
        vizconSemList->next = sem;
        vizconSemList = sem;
    }
    return sem;
}

//Consume one permit from a semaphore, or wait until one is available.
void vcSemWait(vcSem* sem)
{
    semWait(sem);
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
}

//Return the current number of permits from semaphore
int vcSemValue(vcSem* sem)
{
    return semValue(sem);
}

//todo
vcMutex* vcMutexCreate()
{
    vcMutex* mutex;
    if(vizconMutexList == NULL)
    {
        mutex = mutexCreate(vizconCreateName(2, 0));
        mutex->num = 0;
        vizconMutexListInitial = mutex;
        vizconMutexList = mutex;
    }
    else
    {
        mutex = mutexCreate(vizconCreateName(2, vizconMutexList->num + 1));
        mutex->num = vizconMutexList->num + 1;
        vizconMutexList->next = mutex;
        vizconMutexList = mutex;
    }
    return mutex;
}

vcMutex* vcMutexCreateNamed(char* name)
{
    char* mallocName = (char*)malloc(sizeof(char) * (vizconStringLength(name) + 1));
    if (mallocName == NULL) 
    {
        vizconError("vcMutexCreateNamed", 502);
    }
    sprintf(mallocName, "%s", name);
    vcMutex* mutex = mutexCreate(mallocName);
    if(vizconMutexList == NULL)
    {
        mutex->num = 0;
        vizconMutexListInitial = mutex;
        vizconMutexList = mutex;
    }
    else
    {
        mutex->num = vizconMutexList->num + 1;
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