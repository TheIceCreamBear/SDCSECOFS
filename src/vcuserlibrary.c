#include "vcuserlibrary.h"

//Global variables from ourprogram.c
extern CSSem* vcThreadSem;
extern CSSem* vcThreadSemInitial;
extern CSThread* vcThreadList;
extern CSThread* vcThreadListInitial;

//Thread list of threads used to create threads
CSThread* vcThreadCobeginList = NULL;
CSThread* vcThreadCobeginListInitial = NULL;

//Create a thread instance with arguments
//Threads do not begin until vcWaitForCompletion or vcWaitForReturn is called
void vcCobegin(threadFunc func, void* arg)
{
    void** arr = malloc(sizeof(void*)*2);
    arr[0] = func;
    arr[1] = arg;
    void* data = arr;
    CSThread* thread = cobeginThread(data);
    if(vcThreadCobeginList == NULL)
    {
        vcThreadCobeginList = thread;
        vcThreadCobeginListInitial = thread;
    }
    else
    {
        vcThreadCobeginList->next = thread;
        vcThreadCobeginList = thread;
    }
    return;
}

//Start all threads created by vcCobegin
void vcWaitForCompletion()
{
    if(vcThreadCobeginListInitial == NULL)
    {
        return;
    }
    CSSem* tempSem;

    //Release all thread creators and ensure they have all been joined and freed
    semSignal(vcThreadSemInitial);
    while(vcThreadCobeginListInitial != NULL)
    {
        vcThreadCobeginList = vcThreadCobeginListInitial->next;
        joinThread(vcThreadCobeginListInitial);
        freeCSThread(vcThreadCobeginListInitial);
        vcThreadCobeginListInitial = vcThreadCobeginList;
    }

    //Begin to join and free all user threads
    while(vcThreadListInitial != NULL)
    {
        vcThreadList = vcThreadListInitial->next;
        joinThread(vcThreadListInitial);
        freeCSThread(vcThreadListInitial);
        vcThreadListInitial = vcThreadList;
    }

    //Reset semList head and begin to free all user semaphores
    vcThreadSem = vcThreadSemInitial->next;
    semWait(vcThreadSemInitial);
    vcThreadSemInitial->next = NULL;
    while(vcThreadSem != NULL)
    {
        tempSem = vcThreadSem->next;
        semClose(vcThreadSem);
        vcThreadSem = tempSem;
    }
    return;
}

//Start all threads created by vcCobegin and return their results
//Results are stored in a void double pointer
void* vcWaitForReturn()
{
    if(vcThreadCobeginListInitial == NULL)
    {
        return NULL;
    }
    int i = 0;
    CSSem* tempSem;

    //Get number of user created threads and create array for return values
    vcThreadCobeginList = vcThreadCobeginListInitial;
    while(vcThreadCobeginList != NULL)
    {
        i++;
        vcThreadCobeginList = vcThreadCobeginList->next;
    }
    void** arr = malloc(sizeof(void*)*i);
    i = 0;

    //Release all thread creators and ensure they have all been joined and freed
    semSignal(vcThreadSemInitial);
    while(vcThreadCobeginListInitial != NULL)
    {
        vcThreadCobeginList = vcThreadCobeginListInitial->next;
        joinThread(vcThreadCobeginListInitial);
        freeCSThread(vcThreadCobeginListInitial);
        vcThreadCobeginListInitial = vcThreadCobeginList;
    }

    //Begin to join and free all user threads, as well as populate return array
    while(vcThreadListInitial != NULL)
    {
        vcThreadList = vcThreadListInitial->next;
        joinThread(vcThreadListInitial);
        arr[i++] = (void*)vcThreadListInitial->returnVal;
        freeCSThread(vcThreadListInitial);
        vcThreadListInitial = vcThreadList;
    }

    //Reset semList head and begin to free all user semaphores
    vcThreadSem = vcThreadSemInitial->next;
    semWait(vcThreadSemInitial);
    vcThreadSemInitial->next = NULL;
    while(vcThreadSem != NULL)
    {
        tempSem = vcThreadSem->next;
        semClose(vcThreadSem);
        vcThreadSem = tempSem;
    }

    return (void*)arr;
}

//Create a semaphore with a name and maximum permit count
//All semaphores must have a name, and values must be an integer greater than zero
vcSem* vcSemCreate(char* name, int count)
{
    vcSem* sem = semCreate(name, count);
    return sem;
}

//Consume one permit from a semaphore, or wait until one is available.
void vcSemWait(vcSem* sem)
{
    semWait(sem);
    sleepThread(25);
}

//Consume one permit from a sempahore, or return immediately if none are available
void vcSemTryWait(vcSem* sem)
{
    semTryWait(sem);
}

//Release one permit from a semaphore
void vcSemSignal(vcSem* sem)
{
    semSignal(sem);
    sleepThread(25);
}

//Return the current number of permits from semaphore
int vcValue(vcSem* sem)
{
    return semValue(sem);
}