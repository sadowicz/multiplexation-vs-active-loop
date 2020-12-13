#ifndef UTILS_H
#define UTILS_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#define DECY_UNIT 0.1
#define CENTY_UNIT 0.01
#define OCTAL_UNIT 0.125

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int strToInt(char* str, int* errFlag);
double strToDouble(char* str, int* errFlag);

int isPipe(int fd);
int isWriteOpened(int fd);
int isReadOpened(int fd);

int isPipeFull(int fd);
int isPipeEmpty(int fd);

int hibernate(double time);

#endif
