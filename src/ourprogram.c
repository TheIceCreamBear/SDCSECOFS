#include "concurrencylib.h"

THREAD_RET thing1(THREAD_PARAM param) {
    int threadNum = *((int*) param);
    int i;
    for(i = 0; i < 10; i++)
    {
        printf("Inside Thread 1, at loop %d\n", i);
    }
    return 123;
}

THREAD_RET thing2(THREAD_PARAM param) {
    int threadNum = *((int*) param);
    int i;
    for(i = 0; i < 10; i++)
    {
        printf("Inside Thread 2, at loop %d\n", i);
    }
    return 456;
}

int main(int argc, char** argv) {
    // show overwriting of user program

    printf("Program Start: %s\n", __func__);
    printf("Calling userMain...\n");
    userMain();
    printf("Back in %s\n", __func__);

    // create threads
    CSThread* thread1 = createThread(&thing1, (void*)1);
    CSThread* thread2 = createThread(&thing2, (void*)2);

    // wait for threads to finish and join them back into this thread
    joinThread(thread1);
    joinThread(thread2);

    // get return values for printing
    int retVal1 = *((int*) thread1->returnVal);
    char* retVal2 = (char*) thread2->returnVal;

    printf("Thread 1 returns: %d\n", retVal1);
    printf("Thread 2 returns: %s\n", retVal2);

    // free malloced CSThread struct
    freeCSThread(thread1);
    freeCSThread(thread2);

    return 0;
}