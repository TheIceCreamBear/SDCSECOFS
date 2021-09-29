// meta: the following 2 lines would not be in their source code, but appended by us before calling the compiler
#include "useroverwrite.h"
#include "concurrencylib.h"


// meta: the type of this function can be up to us, weather or not it returns an int or nothing. It's name will be overwritten by useroverwrite.h
int main(void) {
    printf("Hello world from the user program function: %s\n", __func__);
    return 0;
}