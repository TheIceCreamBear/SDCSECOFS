#ifndef VCUSERLIBRARY_H
#define VCUSERLIBRARY_H

#include "threads.h"
#include "semaphores.h"
#include "mutexes.h"

#define main userMain
#define vcSem CSSem
#define vcMutex CSMutex

//Lists used to track all threads and semaphores
CSThread* vizconThreadList; //Linked list of all threads
CSThread* vizconThreadListInitial;
CSSem* vizconSemList; //Linked list of all semaphores
CSSem* vizconSemListInitial;
CSMutex* vizconMutexList; //Linked list of all mutexes
CSMutex* vizconMutexListInitial;

//Thread functions
void vcThreadQueue(threadFunc func, void* arg);
void vcThreadStart();
THREAD_RET* vcThreadReturn();
void vcThreadSleep(int milliseconds);

//Semaphore functions
vcSem* vcSemCreate(char* name, int count);
void vcSemWait(vcSem* sem);
int vcSemTryWait(vcSem* sem);
void vcSemSignal(vcSem* sem);
int vcSemValue(vcSem* sem);

//Mutex functions
vcMutex* vcMutexCreate(char* name);
void vcMutexLock(vcMutex* mutex);
int vcMutexTrylock(vcMutex* mutex);
void vcMutexUnlock(vcMutex* mutex);
int vcMutexStatus(vcMutex* mutex);

#endif