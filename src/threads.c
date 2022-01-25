#include "threads.h"

//Create a CSThread that does not start until WaitForCompletion or WaitForReturn is called
//arg parameter must have funcion at head of array
CSThread* threadCreate(void* arg) 
{
    CSThread* thread = (CSThread*)malloc(sizeof(CSThread));
    if (thread == NULL) 
    {
        vizconError("vcThreadQueue", 8);
    }
    thread->next = NULL;
    void** arr = (void**)arg;
    #if defined(_WIN32) // windows
    thread->thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)arr[0], arr[1], CREATE_SUSPENDED, &(thread->id));
    if(thread->thread == NULL)
    {
        int err = (int)GetLastError();
        free(thread);
        free(arg);
        vizconError("vcThreadQueue", err);
    }
    #elif defined(__APPLE__) || defined(__linux__)
    thread->func = arr[0];
    thread->arg = arr[1];
    #endif
    return thread;
}

//Start a given thread that was in a suspended state
void threadStart(CSThread* thread)
{
    #if defined(_WIN32) // windows
    if(ResumeThread(thread->thread) == -1)
    {
        vizconError("vcThreadStart/vcThreadReturn", GetLastError());
    }
    #elif defined(__APPLE__) || defined(__linux__)
    int err = pthread_create(&thread->thread, NULL, thread->func, thread->arg);
    if(err)
    {
        free(thread);
        vizconError("vcThreadQueue", err);
    }
    #endif
}

 //Waits for thread to complete before being joined back into main function
void threadJoin(CSThread* thread) 
{
    #if defined(_WIN32) // windows
    DWORD ret = WaitForSingleObject(thread->thread, INFINITE);
    if(ret == WAIT_FAILED)
    {
        vizconError("vcThreadStart/vcThreadReturn", GetLastError());
    }
    else if(ret == WAIT_OBJECT_0)
    {
        if(!GetExitCodeThread(thread->thread, &thread->returnVal))
        {
            vizconError("vcThreadStart/vcThreadReturn", GetLastError());
        }
    }
    else if(ret == WAIT_ABANDONED)
    {
        vizconError("vcThreadStart/vcThreadReturn", 500);
    }
    else if (ret == WAIT_TIMEOUT)
    {
        vizconError("vcThreadStart/vcThreadReturn", 501);
    }
    #elif defined(__APPLE__) || defined(__linux__)
    int err = pthread_join(thread->thread, &thread->returnVal);
    if(err)
    {
        free(thread);
        vizconError("vcThreadStart/vcThreadReturn", err);
    }
    #endif
}

//Frees all data associated with a CSThread type, including itself
void threadClose(CSThread* thread) 
{
    #ifdef _WIN32
    if(!CloseHandle(thread->thread))
    {
        vizconError("vcThreadStart/vcThreadReturn", GetLastError());
    }
    free(thread);
    #else
    free(thread);
    #endif
}