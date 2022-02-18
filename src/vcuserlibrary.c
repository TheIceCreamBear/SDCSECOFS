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

//Create a thread instance with arguments
//Threads do not begin until vcWaitForCompletion or vcWaitForReturn is called
//Takes additional third parameter for user to assign a name to this thread
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

    //free all user threads
    while(vizconThreadListInitial != NULL)
    {
        vizconThreadList = vizconThreadListInitial->next;
        free(vizconThreadListInitial->name);
        threadClose(vizconThreadListInitial);
        vizconThreadListInitial = vizconThreadList;
    }

    //Free all semaphores
    while(vizconSemListInitial != NULL)
    {
        vizconSemList = vizconSemListInitial->next;
        free(vizconSemListInitial->name);
        semClose(vizconSemListInitial);
        vizconSemListInitial = vizconSemList;
    }

    //Free all mutex locks
    while(vizconMutexListInitial != NULL)
    {
        vizconMutexList = vizconMutexListInitial->next;
        free(vizconMutexListInitial->name);
        mutexClose(vizconMutexListInitial);
        vizconMutexListInitial = vizconMutexList;
    }

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

    //free all user threads
    while(vizconThreadListInitial != NULL)
    {
        vizconThreadList = vizconThreadListInitial->next;
        free(vizconThreadListInitial->name);
        threadClose(vizconThreadListInitial);
        vizconThreadListInitial = vizconThreadList;
    }

    //Free all semaphores
    while(vizconSemListInitial != NULL)
    {
        vizconSemList = vizconSemListInitial->next;
        free(vizconSemListInitial->name);
        semClose(vizconSemListInitial);
        vizconSemListInitial = vizconSemList;
    }

    //Free all mutex locks
    while(vizconMutexListInitial != NULL)
    {
        vizconMutexList = vizconMutexListInitial->next;
        free(vizconMutexListInitial->name);
        mutexClose(vizconMutexListInitial);
        vizconMutexListInitial = vizconMutexList;
    }

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
//Takes additional second parameter for user to assign a name to this semaphore
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

//Consume a number of permits from a semaphore equal to a user-specified number, or wait until they are all available.
void vcSemWaitMult(vcSem* sem, int num)
{
    int i;
    for(i=0; i<num; i++)
    {
        semWait(sem);
    }
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

//Consume a number of permits from a sempahore equal to a user-specified number, or return immediately if all are not available
//Returns 1 if successful, else 0
int vcSemTryWaitMult(vcSem* sem, int num)
{
    int i;
    if(vcSemValue(sem) < num)
    {
        return 0;
    }
    for(i=0; i<num; i++)
    {
        if(!vcSemTryWait(sem))
        {
            for(i=i; i>0; i--)
            {
                vcSemSignal(sem);
            }
            return 0;
        }
    }
    return 1;
}

//Release one permit from a semaphore
void vcSemSignal(vcSem* sem)
{
    semSignal(sem);
}

//Release a number of permits from a sempahore equal to a user-specified number
void vcSemSignalMult(vcSem* sem, int num)
{
    int i;
    for(i=0; i<num; i++)
    {
        semSignal(sem);
    }
}

//Return the current number of permits from semaphore
int vcSemValue(vcSem* sem)
{
    return semValue(sem);
}

//Create a mutex lock in an unlocked state
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

//Create a mutex lock in an unlocked state
//Takes a parameter for users to assign a name to this mutex
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

//Lock a mutex
void vcMutexLock(vcMutex* mutex)
{
    mutexLock(mutex);
}

//lock a mutex, or return immediately if it is not available
//returns 1 if available, 0 if not
int vcMutexTrylock(vcMutex* mutex)
{
    if(mutexTryLock(mutex))
    {
        return 1;
    }
    return 0;
}

//unlock a mutex
void vcMutexUnlock(vcMutex* mutex)
{
    mutexUnlock(mutex);
}

//Check to see if a mutex is unlocked
//Returns 1 of unlocked, 0 if locked
int vcMutexStatus(vcMutex* mutex)
{
    return mutexStatus(mutex);
}