#include "concurrencylib.h"

THREAD_RET thing(THREAD_PARAM param) {
    int i;
    int ret = *(int*)param;
    for(i=0; i<10; i++)
    {
        printf("Inside Thread %d, on loop %d\n", GetCurrentThreadId(), i);
    }
    free(param);
    return ret;
}


int main(int argc, char** argv) {
    // show overwriting of user program
    printf("Program Start: %s\n", __func__);
    printf("Calling userMain...\n");
    userMain();
    printf("Back in %s\n", __func__);

    //create semaphore
    createSemaphore();

    //declare variables
    int i;
    int maxThread = 10;
    void* param;
    CSThread* thread[maxThread];
    
    // create threads
    for(i=0; i<maxThread; i++)
    {
        param = malloc(sizeof(int));
        *(int*)param = i;
        thread[i] = createThread(thing, param);
        printf("Thread %d created\n", thread[i]->id);
    }

    // wait for threads to finish and join them back into this thread
    for(i=0; i<maxThread; i++)
    {
        joinThread(thread[i]);
    }

    // get return values for printing
    for(i=0; i<maxThread; i++)
    {
        printf("Thread %d returns: %d\n", thread[i]->id, thread[i]->returnVal);
    }

    // free malloced CSThread struct
    for(i=0; i<maxThread; i++)
    {
        freeCSThread(thread[i]);
    }

    return 0;
}