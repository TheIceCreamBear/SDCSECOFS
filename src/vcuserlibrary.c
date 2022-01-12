#include "vcuserlibrary.h"

//Lists used to track all threads and semaphores
CSSem* vizconThreadSem = NULL; //Blocks createthread threads, released by waitforcompletion/waitforreturn. Count used for number of threads created
CSThread* vizconThreadList = NULL; //Linked list of all threads
CSThread* vizconThreadListInitial = NULL;
CSSem* vizconSemList = NULL; //Linked list of all semaphores
CSSem* vizconSemListInitial = NULL;
CSThread* vizconCobeginList = NULL; // List of all cobegin threads
CSThread* vizconCobeginListInitial = NULL;

//Create a thread instance with arguments
//Threads do not begin until vcWaitForCompletion or vcWaitForReturn is called
void vcCobegin(threadFunc func, void* arg)
{
    if(vizconThreadSem == NULL)
    {
        vizconThreadSem = semCreate("/vizconThreadSem", 1);
        semWait(vizconThreadSem);
    }
    void** arr = malloc(sizeof(void*)*2);
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

    //Release all thread creators and ensure they have all been joined and freed
    semSignal(vizconThreadSem);
    while(vizconCobeginListInitial != NULL)
    {
        vizconCobeginList = vizconCobeginListInitial->next;
        joinThread(vizconCobeginListInitial);
        freeCSThread(vizconCobeginListInitial);
        vizconCobeginListInitial = vizconCobeginList;
    }

    //Begin to join and free all user threads
    while(vizconThreadListInitial != NULL)
    {
        vizconThreadList = vizconThreadListInitial->next;
        joinThread(vizconThreadListInitial);
        freeCSThread(vizconThreadListInitial);
        vizconThreadListInitial = vizconThreadList;
    }

    //Free all semaphores
    while(vizconSemList != NULL)
    {
        vizconSemList = vizconSemListInitial->next;
        semClose(vizconSemListInitial);
        vizconSemListInitial = vizconSemList;
    }
    semClose(vizconThreadSem);

    return;
}

//Start all threads created by vcCobegin and return their results
//Results are stored in a void double pointer
void* vcWaitForReturn()
{
    if(vizconCobeginListInitial == NULL)
    {
        return NULL;
    }
    int i = 0;
    void** arr = malloc(sizeof(void*)*vizconThreadSem->count);

    //Release all thread creators and ensure they have all been joined and freed
    semSignal(vizconThreadSem);
    while(vizconCobeginListInitial != NULL)
    {
        vizconCobeginList = vizconCobeginListInitial->next;
        joinThread(vizconCobeginListInitial);
        freeCSThread(vizconCobeginListInitial);
        vizconCobeginListInitial = vizconCobeginList;
    }

    //Begin to join and free all user threads, as well as populate return array
    while(vizconThreadListInitial != NULL)
    {
        vizconThreadList = vizconThreadListInitial->next;
        joinThread(vizconThreadListInitial);
        arr[i++] = (void*)vizconThreadListInitial->returnVal;
        freeCSThread(vizconThreadListInitial);
        vizconThreadListInitial = vizconThreadList;
    }

    //Free all semaphores
    while(vizconSemList != NULL)
    {
        vizconSemList = vizconSemListInitial->next;
        semClose(vizconSemListInitial);
        vizconSemListInitial = vizconSemList;
    }
    semClose(vizconThreadSem);

    return (void*)arr;
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