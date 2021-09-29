#include "concurrencylib.h"

int main(int argc, char** argv) {
    printf("Program Start: %s\n", __func__);
    printf("Calling userMain...\n");
    userMain();
    printf("Back in %s\n", __func__);
    printf("Exiting\n");
    return 0;
}