#include "utils.h"
#include "vcuserlibrary.h"

//Handles error from concurrencylib and vcuserlibrary
void vizconError(char* func, int err)
{
    vizconAbort();
    printf("\nError from %s\n", func);
    #if defined(_WIN32) // windows
    LPSTR message;
    if(err < 500)
    {
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message, 0, NULL);
        printf("system error ");
    }
    #elif defined(__linux__) || defined(__APPLE__)
    char* message;
    if(err < 500)
    {
        message = strerror(err);
        printf("errno ");
    }
    #endif
    if(err >= 500)
    {
        printf("vizcon error ");
        switch(err)
        {
            case 500:
            {
                message = "A thread terminated without releasing its mutex lock.";
                break;
            }
            case 501:
            {
                message = "An unexpected wait timeout occurred.";
                break;
            }
            default:
            {
                message = "An unknown error has occurred.";
            }
        }
    }
    printf("code %d: %s\n", err, message);
    exit(0);
}

//Kill all threads and free all concurrency structures
void vizconAbort()
{
    #if defined(_WIN32) // windows
    DWORD dwExitCode = 0;
    DWORD id = GetCurrentThreadId();
    while(vizconThreadListInitial != NULL)
    {
        if(vizconThreadListInitial->id != id)
        {
            TerminateThread(vizconThreadListInitial->thread, dwExitCode);
        }
        vizconThreadListInitial = vizconThreadListInitial->next;
    }
    #elif defined(__linux__) || defined(__APPLE__)
    int id = pthread_self();
    while(vizconThreadListInitial != NULL)
    {
        if((int)vizconThreadListInitial->thread != id)
        {
            pthread_cancel(vizconThreadListInitial->thread);
        }
        vizconThreadListInitial = vizconThreadListInitial->next;
    }
    #endif

    vizconFree();
}

//Free all vizcon data
void vizconFree()
{
    //free all user threads
    while(vizconThreadListInitial != NULL)
    {
        vizconThreadList = vizconThreadListInitial->next;
        threadClose(vizconThreadListInitial);
        vizconThreadListInitial = vizconThreadList;
    }

    //Free all semaphores
    while(vizconSemListInitial != NULL)
    {
        vizconSemList = vizconSemListInitial->next;
        semClose(vizconSemListInitial);
        vizconSemListInitial = vizconSemList;
    }

    //Free all mutex locks
    while(vizconMutexListInitial != NULL)
    {
        vizconMutexList = vizconMutexListInitial->next;
        mutexClose(vizconMutexListInitial);
        vizconMutexListInitial = vizconMutexList;
    }
}