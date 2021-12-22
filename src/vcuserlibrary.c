#include "vcuserlibrary.h"

extern CSSem* vcThreadSem;
extern CSSem* vcThreadSemInitial;
extern CSThread* vcThreadListInitial;
extern int numThreads;

void vcCobegin(threadFunc func, void* arg)
{
    void** arr = malloc(sizeof(void*)*2);
    arr[0] = func;
    arr[1] = arg;
    cobeginThread(arr);
    numThreads++;
    return;
}

void vcWaitForCompletion()
{
    if(numThreads == 0)
    {
        return;
    }
    int numThreadIter = numThreads;
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

void** vcWaitForReturn()
{
    if(numThreads == 0)
    {
        return NULL;
    }
    void** arr = malloc(sizeof(void*)*numThreads);
    int i = 0;
    int numThreadIter = numThreads;
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

vcSem* vcSemCreate(char* name, int count)
{
    vcSem* sem = semCreate(name, count);
    return sem;
}

void vcSemWait(vcSem* sem)
{
    semWait(sem);
}

void vcSemTryWait(vcSem* sem)
{
    semTryWait(sem);
}

void vcSemSignal(vcSem* sem)
{
    semSignal(sem);
}

int vcValue(vcSem* sem)
{
    return semValue(sem);
}