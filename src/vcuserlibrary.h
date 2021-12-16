#include "concurrencylib.h"

void vcCobegin(threadFunc func, void* arg);
void vcWaitForCompletion();
void** vcWaitForReturn();