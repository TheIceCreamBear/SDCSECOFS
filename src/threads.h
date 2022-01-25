#include "utils.h"

#ifdef _WIN32 // window's libraries and definitions
typedef DWORD (*threadFunc) (LPVOID param);
#define THREAD_FUNC_RET DWORD WINAPI
#define THREAD_RET DWORD
#define THREAD_PARAM LPVOID
#elif __linux__ || __APPLE__ //Linux and MacOS's libraries and definitions
typedef void* (*threadFunc) (void* param);
#define THREAD_FUNC_RET void*
#define THREAD_RET void*
#define THREAD_PARAM void*
#endif

// concurrency simulator threading structure
typedef struct CSThread {
    #if defined(_WIN32) // windows
    HANDLE thread;
    THREAD_RET id;
    #elif defined(__APPLE__) || defined(__linux__)
    pthread_t thread;
    threadFunc func;
    void* arg;
    #endif
    THREAD_RET returnVal;
    struct CSThread* next;
} CSThread;

//Thread functions 
CSThread* threadCreate(void* arg);
void threadStart(CSThread* thread);
void threadJoin(CSThread* thread);
void threadClose(CSThread* thread);