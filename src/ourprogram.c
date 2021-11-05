#include "concurrencylib.h"

THREAD_RET thing(THREAD_PARAM param) {
    int i;
    #ifdef _WIN32
    DWORD ret;
    ret = *(DWORD*)param;
    free(param);
    #else
    void* ret;
    ret = param;
    #endif
    for(i=0; i<5; i++)
    {
        if(wait())
        {
            printf("Thread %d working on loop %d. Sem decreased to %d\n", 
            #ifdef _WIN32 
            GetCurrentThreadId(), i, *semCount);
            Sleep(1*1000);
            #else 
            (int)pthread_self(), i, *semCount);
            sleep(1);
            #endif
            signal();
            printf("Thread %d completed loop %d. Sem increased to %d\n",
            #ifdef _WIN32 
            GetCurrentThreadId(), i, *semCount);
            #else 
            (int)pthread_self(), i, *semCount);
            #endif
        }
        else
        {
            printf("Thread %d held on loop %d due to Sem <= %d\n",
            #ifdef _WIN32 
            GetCurrentThreadId(), i--, *semCount);
            Sleep(1*1000);
            #else 
            (int)pthread_self(), i--, *semCount);
            sleep(1);
            #endif
        }
    }
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
    printf("Semaphore created with max value %d\n", MAX_SEM);

    //declare variables
    int i;
    int maxThread = 5;
    void* param;
    CSThread* thread[maxThread];

    // create threads
    for(i=0; i<maxThread; i++)
    {
        param = malloc(sizeof(int));
        *(int*)param = i;
        thread[i] = createThread(thing, param);
        printf("Thread %d created\n",
        #ifdef _WIN32 
        thread[i]->id
        #else 
        (int)thread[i]->thread
        #endif 
        );
    }

    // wait for threads to finish and join them back into this thread
    for(i=0; i<maxThread; i++)
    {
        joinThread(thread[i]);
    }
    // get return values for printing
    for(i=0; i<maxThread; i++)
    {
        printf("Thread %d returns: %d\n", 
        #ifdef _WIN32 
        thread[i]->id, thread[i]->returnVal);
        #else 
        (int)thread[i]->thread, *(int*)thread[i]->returnVal);
        #endif
    }

    // free malloced CSThread struct
    for(i=0; i<maxThread; i++)
    {
        freeCSThread(thread[i]);
    }
    closeSemaphore(sem);

    printf("\nProcess ended with exit code 0\n");
    return 0;
}