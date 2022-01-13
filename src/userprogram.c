// meta: the following 2 lines would not be in their source code, but appended by us before calling the compiler
#include "useroverwrite.h"
#include "vcuserlibrary.h"  

#include <stdio.h>

vcSem *fork1, *fork2, *fork3, *fork4, *fork5, *room;
int m = 20;
vcMutex *lol;

THREAD_RET Phil1(THREAD_PARAM param)
{
    int i;
    vcMutexLock(lol);
    for(i=0; i<m; i++)
    {
        vcSemWait(room);
        vcSemWait(fork1);
        vcSemWait(fork2);
        printf("P1 is eating: loop %d\n", i);
        vcSemSignal(fork2);
        vcSemSignal(fork1);
        vcSemSignal(room);
    }
    vcMutexUnlock(lol);
    return (THREAD_RET)param;
}

THREAD_RET Phil2(THREAD_PARAM param)
{
    int i;
    vcMutexLock(lol);
    for(i=0; i<m; i++)
    {
        vcSemWait(room);
        vcSemWait(fork2);
        vcSemWait(fork3);
        printf("P2 is eating: loop %d\n", i);
        vcSemSignal(fork3);
        vcSemSignal(fork2);
        vcSemSignal(room);
    }
    return (THREAD_RET)param;
}

THREAD_RET Phil3(THREAD_PARAM param)
{
    int i;
    vcMutexLock(lol);
    for(i=0; i<m; i++)
    {
        vcSemWait(room);
        vcSemWait(fork3);
        vcSemWait(fork4);
        printf("P3 is eating: loop %d\n", i);
        vcSemSignal(fork4);
        vcSemSignal(fork3);
        vcSemSignal(room);
    }
    return (THREAD_RET)param;
}

THREAD_RET Phil4(THREAD_PARAM param)
{
    int i;
    vcMutexLock(lol);
    for(i=0; i<m; i++)
    {
        vcSemWait(room);
        vcSemWait(fork4);
        vcSemWait(fork5);
        printf("P4 is eating: loop %d\n", i);
        vcSemSignal(fork5);
        vcSemSignal(fork4);
        vcSemSignal(room);
    }
    return (THREAD_RET)param;
}

THREAD_RET Phil5(THREAD_PARAM param)
{
    int i;
    vcMutexLock(lol);
    for(i=0; i<m; i++)
    {
        vcSemWait(room);
        vcSemWait(fork5);
        vcSemWait(fork1);
        printf("P5 is eating: loop %d\n", i);
        vcSemSignal(fork1);
        vcSemSignal(fork5);
        vcSemSignal(room);
    }
    return (THREAD_RET)param;
}

// meta: the type of this function can be up to us, weather or not it returns an int or nothing. It's name will be overwritten by useroverwrite.h
int main(void) {
    fork1 = vcSemCreate("fork1", 1);
    fork2 = vcSemCreate("fork2", 1);
    fork3 = vcSemCreate("fork3", 1);
    fork4 = vcSemCreate("fork4", 1);
    fork5 = vcSemCreate("fork5", 1);
    room = vcSemCreate("room", 4);
    
    lol = vcMutexCreate("hi");
    printf("hello\n");

    vcCobegin(Phil1, (void*)"This");
    vcCobegin(Phil2, (void*)"is");
    vcCobegin(Phil3, (void*)"a");
    vcCobegin(Phil4, (void*)"concurrency");
    vcCobegin(Phil5, (void*)"test");
    //vcWaitForCompletion();
    void* arr = vcWaitForReturn();
    int i;
    for(i = 0; i < 5; i++)
    {
        printf("Thread retrieved: %s\n", (char*)(((void**)arr)[i]));
    }
    return 0;
}