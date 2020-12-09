#include "utils.h"

int strToInt(char* str, int* errFlag)
{
	char* endptr = NULL;
	errno = 0;

	long res = strtol(str, &endptr, 0);

	if(errno || *endptr)
	{
		fprintf(stderr, "ERROR: strToInt: Unable to convert given string ( %s ) to int number\n", str);
		*errFlag = 1;
	}

	return (int)res;
}

int isPipe(int fd)
{
	int res = 0;
	
	struct stat statbuf;

	if(fstat(fd, &statbuf) != -1)
		res = S_ISFIFO(statbuf.st_mode);	// TODO Check if returns true also for pipes
	else
		fprintf(stderr, "ERROR: isPipe: Unable to get file stats\n");

	return res;
}

int isWriteOpened(int fd)
{
	int res = 0;
	int descFlags = fcntl(fd, F_GETFL);
	
	if(descFlags != -1)
		res = ((descFlags & O_ACCMODE) == O_WRONLY);
	else
		fprintf(stderr, "ERROR: isWriteOpened: Unable to get file flags\n");

	return res;
}

int isReadOpened(int fd)
{
	int res = 0;
	int descFlags = fcntl(fd, F_GETFL);
	
	if(descFlags != -1)
		res = ((descFlags & O_ACCMODE) == O_RDONLY);
	else
		fprintf(stderr, "ERROR: isReadOpened: Unable to get file flags\n");

	return res;
}
