// meta: the following 2 lines would not be in their source code, but appended by us before calling the compiler
#include "useroverwrite.h"
#include "vcuserlibrary.h"

#include <stdio.h>

THREAD_RET hello(THREAD_PARAM i)
{
    printf("%s\n", i);
    DWORD ret = ((char*)i)[0];
    return ret;
}

// meta: the type of this function can be up to us, weather or not it returns an int or nothing. It's name will be overwritten by useroverwrite.h
int main(void) {
    void** arr;
    vcCobegin(hello, (void*)"This");
    vcCobegin(hello, (void*)"is");
    vcCobegin(hello, (void*)"a");
    vcCobegin(hello, (void*)"concurrency");
    vcCobegin(hello, (void*)"test");
    arr = vcWaitForReturn();
    int i;
    for(i = 0; i < 5; i++)
    {
        printf("Thread retrieved: %c\n", (char*)arr[i]);
    }
    return 0;
}