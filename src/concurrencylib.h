#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32 // windows
#include <windows.h>
typedef DWORD (*threadFunc) (LPVOID param);
#define THREAD_FUNC_RET DWORD WINAPI
#define THREAD_RET DWORD
#define THREAD_PARAM LPVOID
#define SEM_TYPE HANDLE
#define SEM_NAME LPCSTR
#define SEM_VALUE LONG
#elif __linux__ || __APPLE__ //Linux and MacOS
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
typedef void* (*threadFunc) (void* param);
#define THREAD_FUNC_RET void*
#define THREAD_RET void*
#define THREAD_PARAM void*
#define SEM_TYPE sem_t*
#define SEM_NAME const char*
#define SEM_VALUE unsigned int
#endif

// meta: proto type for the compiled name of the user function so we have access to it in our program
int userMain(void); 

// concurrency simulator thread
typedef struct CSThread {
    #if defined(_WIN32) // windows
    HANDLE thread;
    THREAD_RET id;
    #elif defined(__APPLE__) || defined(__linux__)
    pthread_t thread;
    #endif
    THREAD_RET returnVal;
    struct CSThread* next;
} CSThread;

// concurrency simulator semaphore
typedef struct CSSem {
    SEM_TYPE sem;
    SEM_VALUE count;
    struct CSSem* next;
} CSSem;

//Thread functions
void cobeginThread(void** arg);
CSThread* createThread(void** arg);
int joinThread(CSThread* thread);
void freeCSThread(CSThread* thread);

//Semaphore functions
CSSem* semCreate(SEM_NAME name, SEM_VALUE maxValue);
int semSignal(CSSem* sem);
int semWait(CSSem* sem);
int semTryWait(CSSem* sem);
int semValue(CSSem* sem);
int semClose(CSSem* sem);