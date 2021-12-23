#include "concurrencylib.h"

CSSem* vcThreadSem; //Linked list of all semaphores
CSSem* vcThreadSemInitial; //First item blocks threads being created by vcCobegin, is released by waitforcompletion or waitforreturn
CSThread* vcThreadList; //Linked list of all threads
CSThread* vcThreadListInitial; //First item contains number of threads created by user in returnVal

int main(int argc, char** argv) {
    // show overwriting of user program
    errno = 0;

    //Create a null thread list
    vcThreadList = (CSThread*)malloc(sizeof(CSThread));
    vcThreadList->thread = NULL;
    vcThreadList->next = NULL;
    vcThreadList->returnVal = 0;
    vcThreadListInitial = vcThreadList;

    //Create thread creation semaphore
    vcThreadSem = NULL;
    vcThreadSem = semCreate("/vcThreadSem", 1);
    vcThreadSemInitial = vcThreadSem;
    semWait(vcThreadSem);
    
    printf("Program Start: %s\n", __func__);
    printf("Calling userMain...\n\n");
    userMain();
    printf("\nBack in %s\n", __func__);
    return 0;
}