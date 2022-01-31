#include "utils.h"

#ifdef _WIN32 // window's libraries and definitions
    #define MUTEX_TYPE HANDLE
#elif __linux__ || __APPLE__ //Linux and MacOS's libraries and definitions
    #define MUTEX_TYPE pthread_mutex_t*
#endif

// concurrency simulator mutex structure
typedef struct CSMutex {
    MUTEX_TYPE mutex;
    char* name;
    int num;
    struct CSMutex* next;
} CSMutex;

//Mutex functions
CSMutex* mutexCreate(char* name);
void mutexLock(CSMutex* mutex);
int mutexTryLock(CSMutex* mutex);
void mutexUnlock(CSMutex* mutex);
void mutexClose(CSMutex* mutex);
int mutexStatus(CSMutex* mutex);