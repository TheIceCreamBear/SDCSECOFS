#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// Thing 1 returns an integer
void *thing1(void *ptr)
{
    int threadNumber = (int) ptr;
    printf("Inside Thread %d, doing thing 1\n", threadNumber);

    return (void *)123;
}

// Thing 2 returns a string
void *thing2(void *ptr)
{
    int threadNumber = (int) ptr;
    printf("Inside Thread %d, doing thing 2\n", threadNumber);

    return (void *)"abc";    
}

int main(int argc, char **argv)
{
    // Initializes the threads
    pthread_t thread1, thread2;

    // The return value of each thread
    void *ret_from_thread1, *ret_from_thread2;

    // Creates the thread, passing in the thread variable, the function and the parameter
    pthread_create(&thread1, NULL, thing1, (void*) 1);
    pthread_create(&thread2, NULL, thing2, (void*) 2);

    // Waits for the threads to finish before continuing, also storing their return value
    pthread_join(thread1, &ret_from_thread1);
    pthread_join(thread2, &ret_from_thread2);

    // Variables to store the return values in the appropriate types
    int retVal1 = (int)ret_from_thread1;
    char *retVal2 = (char*)ret_from_thread2;

    printf("Thread 1 returns: %d\n", retVal1);
    printf("Thread 2 returns: %s\n", retVal2);

    return 0;
}