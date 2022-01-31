#include <stdio.h>

// meta: proto type for the compiled name of the user function so we have access to it in our program
int userMain(void);

int main(int argc, char** argv) 
{
    //Begin the user program
    printf("Program Start: %s\n", __func__);
    printf("Calling userMain...\n\n");
    userMain();
    printf("\nBack in %s\n", __func__);
    return 0;
}