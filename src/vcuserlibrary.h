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
void vcThreadQueueNamed(threadFunc func, void* arg, char* name);
void vcThreadStart();
THREAD_RET* vcThreadReturn();
void vcThreadSleep(int milliseconds);

//Semaphore functions
vcSem* vcSemCreate(int count);
vcSem* vcSemCreateNamed(int count, char* name);
void vcSemWait(vcSem* sem);
void vcSemWaitMult(vcSem* sem, int num);
int vcSemTryWait(vcSem* sem);
int vcSemTryWaitMult(vcSem* sem, int num);
void vcSemSignal(vcSem* sem);
void vcSemSignalMult(vcSem* sem, int num);
int vcSemValue(vcSem* sem);

//Mutex functions
vcMutex* vcMutexCreate();
vcMutex* vcMutexCreateNamed(char* name);
void vcMutexLock(vcMutex* mutex);
int vcMutexTrylock(vcMutex* mutex);
void vcMutexUnlock(vcMutex* mutex);
int vcMutexStatus(vcMutex* mutex);

#endif