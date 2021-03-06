// meta: the following 2 lines would not be in their source code, but appended by us before calling the compiler
#include "vcuserlibrary.h"

int b1, b2, b3;
vcSem *empty1, *empty2, *empty3, *full1, *full2, *full3;

THREAD_RET Producer(THREAD_PARAM param)
{
    int r, i;
    srand(vcThreadId());
    for(i=0; i<20; i++)
    {
        r = rand() % 100;
        vcSemWait(empty1);
        b1 = r;
        vcSemSignal(full1);
    }
    return 1;
}

THREAD_RET CheckEven(THREAD_PARAM param)
{
    int n;
    while(1)
    {
        vcSemWait(full1);
        n = b1;
        vcSemSignal(empty1);
        vcSemWait(empty2);
        b2 = n;
        vcSemSignal(full2);
        if(n % 2 == 0)
        {
            vcSemWait(empty2);
            b2 = 0;
            vcSemSignal(full2);
        }
    }
    return 1;
}

THREAD_RET CheckLine(THREAD_PARAM param)
{
    int n, count = 1;
    while(1)
    {
        vcSemWait(full2);
        n = b2;
        vcSemSignal(empty2);
        vcSemWait(empty3);
        b3 = n;
        vcSemSignal(full3);
        if(count % 5 == 0)
        {
            vcSemWait(empty3);
            b3 = -1;
            vcSemSignal(full3);
        }
        count = count + 1;
    }
    return 1;
}

THREAD_RET Consumer(THREAD_PARAM param)
{
    int k;
    while(1)
    {
        vcSemWait(full3);
        k = b3;
        vcSemSignal(empty3);
        if(k < 0)
        {
            printf("\n");
        }
        else
        {
            printf("%d ", k);
        }
    }
    return 1;
}

int main(void) 
{
    empty1 = vcSemCreate(1);
    empty2 = vcSemCreate(1);
    empty3 = vcSemCreate(1);
    full1 = vcSemCreateInitial(0, 1);
    full2 = vcSemCreateInitial(0, 1);
    full3 = vcSemCreateInitial(0, 1);
    vcThreadQueue(Producer, NULL);
    vcThreadQueue(CheckEven, NULL);
    vcThreadQueue(CheckLine, NULL);
    vcThreadQueue(Consumer, NULL);
    vcThreadStart();
    return 1;
}