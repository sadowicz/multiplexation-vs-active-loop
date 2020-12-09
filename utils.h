#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int strToInt(char* str, int* errFlag);

int isPipe(int fd);
int isWriteOpened(int fd);
int isReadOpened(int fd);

#endif
