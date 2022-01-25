#ifndef UTILS_H
#define UTILS_H
//universal libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32 // window's libraries and definitions
#include <windows.h>
#elif __linux__ || __APPLE__ //Linux and MacOS's libraries and definitions
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#endif

//Other functions
void vizconAbort();
void vizconError(char* func, int err);
void vizconFree();

#endif