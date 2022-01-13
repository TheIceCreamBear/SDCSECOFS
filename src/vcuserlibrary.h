#ifndef VCUSERLIBRARY_H
#define VCUSERLIBRARY_H
#include "concurrencylib.h"

#define vcSem CSSem

//Thread functions
void vcCobegin(threadFunc func, void* arg);
void vcWaitForCompletion();
void* vcWaitForReturn();

//Semaphore functions
vcSem* vcSemCreate(char* name, int count);
void vcSemWait(vcSem* sem);
int vcSemTryWait(vcSem* sem);
void vcSemSignal(vcSem* sem);
int vcValue(vcSem* sem);
#endif