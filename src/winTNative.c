#include <windows.h>
#include <stdio.h>

// Thing 1 returns an integer
DWORD WINAPI thing1(LPVOID lpParameter)
{
    int threadNumber = (int) lpParameter;
    printf("Inside Thread %d, doing thing 1\n", threadNumber);

    return (void *)123;
}

// Thing 2 returns a string
DWORD WINAPI thing2(LPVOID lpParameter)
{
    int threadNumber = (int) lpParameter;
    printf("Inside Thread %d, doing thing 2\n", threadNumber);

    return (void *)"abc";
}

int main (int argc, char *argv[])
{
    // Initializes the threads
    HANDLE thread1;
    HANDLE thread2;

    // Return value of each thread
    DWORD ret_from_thread1, ret_from_thread2;

    // Creates the thread, passing in the function and the parameter
    thread1 = CreateThread(0, 0, thing1, (void *) 1, 0, 0);
    thread2 = CreateThread(0, 0, thing2, (void *) 2, 0, 0);

    // Waits for the threads to finish before continuing
    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);

    // Gets the return value of each thread
    GetExitCodeThread(thread1, &ret_from_thread1);
    GetExitCodeThread(thread2, &ret_from_thread2);

    // Variables to store the return values in the appropriate types
    int retVal1 = (int) ret_from_thread1;
    char *retVal2 = (char *) ret_from_thread2;

    // Closes the threads
    CloseHandle(thread1);
    CloseHandle(thread2);

    printf("Thread 1 returns %d\n", retVal1);
    printf("Thread 1 returns %s\n", retVal2);

    return 0;
}