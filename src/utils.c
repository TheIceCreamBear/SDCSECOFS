#include "utils.h"

//Creates a name for a concurrency structure
char* vizconCreateName(int type, int value)
{
    int i = 1, temp = value/10;
    while(temp != 0)
    {
        i++;
        temp = temp / 10;
    }
    char* ret = (char*)malloc(sizeof(char) * (11+i));
    if(ret == NULL)
    {
        vizconError("create function", 502);
    }
    switch(type)
    {
        case 0:
        {
            sprintf(ret, "Thread %d", value);
            return ret;
        }
        case 1:
        {
            sprintf(ret, "Semaphore %d", value);
            return ret;
        }
        case 2:
        {
            sprintf(ret, "Mutex %d", value);
            return ret;
        }
        default:
        {
            return NULL;
        }
    }
}

//Handles error from concurrencylib and vcuserlibrary
void vizconError(char* func, int err)
{
    int maxLen = 200;
    char message[maxLen];
    sprintf(message, "\nError from %s\n", func);
    #if defined(_WIN32) // windows
    LPSTR errorMessage;
    if(err < 500)
    {
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&errorMessage, 0, NULL);
        sprintf(message, "%ssystem error", message);
    }
    #elif defined(__linux__) || defined(__APPLE__)
    char* errorMessage;
    if(err < 500)
    {
        sprintf(message, "%serrno code %d", message, err);
        errno = err; 
        perror(message);
        exit(0);
    }
    #endif
    if(err >= 500)
    {
        sprintf(message, "%svizcon error", message);
        switch(err)
        {
            case 500:
            {
                errorMessage = "A thread terminated without releasing its mutex lock.";
                break;
            }
            case 501:
            {
                errorMessage = "An unexpected wait timeout occurred.";
                break;
            }
            case 502:
            {
                errorMessage = "Not enough memory resources are available to process this command.";
                break;
            }
            case 503:
            {
                errorMessage = "A semaphore was created with a non-whole number value.";
                break;
            }
            case 504:
            {
                errorMessage = "There was an error saving the internal mutex name.";
                break;
            }
            case 505:
            {
                errorMessage = "There was an error creating a thread with the given name.";
                break;
            }
            case 506:
            {
                errorMessage = "There was an error closing a thread.";
                break;
            }
            case 510:
            {
                errorMessage = "A thread attempted to unlock an already-unlocked mutex.";
                break;
            }
            case 511:
            {
                errorMessage = "A thread attempted to lock a mutex that it already locked.";
                break;
            }
            case 512:
            {
                errorMessage = "A thread attempted to unlock an mutex that was locked by another thread.";
                break;
            }
            default:
            {
                errorMessage = "An unknown error has occurred.";
            }
        }
    }
    sprintf(message, "%s code %d: %s\n", message, err, errorMessage);
    printf("%s", message);
    exit(0);
}