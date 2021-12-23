#include "vcuserlibrary.h"

//Global variables from ourprogram.c
extern CSSem* vcThreadSem;
extern CSSem* vcThreadSemInitial;
extern CSThread* vcThreadListInitial;

//Create a thread instance with arguments
//Threads do not begin until vcWaitForCompletion or vcWaitForReturn is called
void vcCobegin(threadFunc func, void* arg)
{
    void** arr = malloc(sizeof(void*)*2);
    arr[0] = func;
    arr[1] = arg;
    cobeginThread(arr);
    vcThreadListInitial->returnVal = vcThreadListInitial->returnVal + 1;
    return;
}

//Start all threads created by vcCobegin
void vcWaitForCompletion()
{
    if(vcThreadListInitial->returnVal == 0)
    {
        return;
    }
    int numThreadIter = vcThreadListInitial->returnVal;
    CSThread* list = vcThreadListInitial;
    CSThread* tempList;
    semSignal(vcThreadSemInitial);
    while(list->next == NULL){;}
    tempList = list->next;
    freeCSThread(list);
    list = tempList;
    vcThreadListInitial = NULL;
    while(numThreadIter > 1)
    {
        joinThread(list);
        while(list->next == NULL){;}
        tempList = list->next;
        freeCSThread(list);
        list = tempList;
        numThreadIter--;
    }
    joinThread(list);
    freeCSThread(list);
    while(vcThreadSemInitial != NULL)
    {
        vcThreadSem = vcThreadSemInitial->next;
        semClose(vcThreadSemInitial);
        vcThreadSemInitial = vcThreadSem;
    }
    return;
}

//Start all threads created by vcCobegin and return their results
//Results are stored in a void double pointer
void** vcWaitForReturn()
{
    if(vcThreadListInitial->returnVal == 0)
    {
        return NULL;
    }
    void** arr = malloc(sizeof(void*)*((int)vcThreadListInitial->returnVal));
    int i = 0;
    int numThreadIter = vcThreadListInitial->returnVal;
    CSThread* list = vcThreadListInitial;
    CSThread* tempList;
    semSignal(vcThreadSemInitial);
    while(list->next == NULL){;}
    tempList = list->next;
    freeCSThread(list);
    list = tempList;
    vcThreadListInitial = NULL;
    while(numThreadIter > 1)
    {
        joinThread(list);
        arr[i] = (void*)list->returnVal;
        i++;
        while(list->next == NULL){;}
        tempList = list->next;
        freeCSThread(list);
        list = tempList;
        numThreadIter--;
    }
    joinThread(list);
    arr[i] = (void*)list->returnVal;
    freeCSThread(list);
    while(vcThreadSemInitial != NULL)
    {
        vcThreadSem = vcThreadSemInitial->next;
        semClose(vcThreadSemInitial);
        vcThreadSemInitial = vcThreadSem;
    }
    return arr;
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
    sleepThread(25);
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