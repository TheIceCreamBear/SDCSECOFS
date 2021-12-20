#include "concurrencylib.h"

CSSem* vcThreadSem; //Blocks threads being created by vcCobegin, is released by waitforcompletion or waitforreturn
CSThread* vcThreadList;
CSThread* vcThreadListInitial;
int numThreads;

int main(int argc, char** argv) {
    // show overwriting of user program
    errno = 0;

    //Create a null thread list
    vcThreadList = (CSThread*)malloc(sizeof(CSThread));
    vcThreadList->thread = NULL;
    vcThreadList->next = NULL;
    vcThreadListInitial = vcThreadList;

    //Create thread creation semaphore
    vcThreadSem = semCreate("/vcThreadSem", 1);
    semWait(vcThreadSem);
    
    printf("Program Start: %s\n", __func__);
    printf("Calling userMain...\n\n");
    userMain();
    printf("\nBack in %s\n", __func__);
    return 0;
}