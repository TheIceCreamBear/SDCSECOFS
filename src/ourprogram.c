#include "concurrencylib.h"

CSSem* vcThreadSem = NULL; //Linked list of all semaphores
CSSem* vcThreadSemInitial = NULL; //First item blocks threads being created by vcCobegin, is released by waitforcompletion or waitforreturn
CSThread* vcThreadList = NULL; //Linked list of all threads
CSThread* vcThreadListInitial = NULL;

int main(int argc, char** argv) {

    //Create thread creation semaphore
    vcThreadSem = semCreate("/vcThreadSem", 1);
    vcThreadSemInitial = vcThreadSem;
    semWait(vcThreadSem);

    //Begin the user program
    printf("Program Start: %s\n", __func__);
    printf("Calling userMain...\n\n");
    userMain();
    printf("\nBack in %s\n", __func__);
    return 0;
}