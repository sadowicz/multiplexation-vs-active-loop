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

double strToDouble(char* str, int* errFlag)
{
	char* endptr = NULL;
	errno = 0;

	double res = strtod(str, &endptr);

	if(errno || *endptr)
	{
		fprintf(stderr, "ERROR: strToDouble: Unable to convert given string ( %s ) to double number\n", str);
		*errFlag = 1;
	}

	return res;
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

int isPipeFull(int fd)
{
	int res = 0;
	int pipeCapacity = fcntl(fd, F_GETPIPE_SZ);

	if(pipeCapacity != -1)
	{
		int pipeSize = -1;
		
		if(ioctl(fd, FIONREAD, &pipeSize) != -1)
			res = (pipeSize == pipeCapacity);
		else
			fprintf(stderr, "ERROR: isPipeFull: Unable to get pipe size\n");
	}
	else
		fprintf(stderr, "ERROR: isPipeFull: Unable to get pipe capacity\n");

	return res;
}

int isPipeEmpty(int fd)
{
	int res = 0;

	int pipeSize = -1;
		
	if(ioctl(fd, FIONREAD, &pipeSize) != -1)
		res = (!pipeSize);
	else
		fprintf(stderr, "ERROR: isPipeFull: Unable to get pipe size\n");

	return res;
}

int hibernate(double time)
{
	int res = 1;
	
	long nsec = (long)(time * 1000000000);
	long sec = (long)(nsec / 1000000000);
	nsec = nsec - (sec * 1000000000);

	struct timespec ts = { .tv_sec = sec, .tv_nsec = nsec };

	if(nanosleep(&ts, NULL) == -1)
	{
		res = 0;
		fprintf(stderr, "ERROR: hibernate: Unable to wait for ( %f ) seconds\n", time);
	}

	return res;
}
