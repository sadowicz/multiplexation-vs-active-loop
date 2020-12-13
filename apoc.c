#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 2
#endif

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "utils.h"

int getArgs(int argc, char** argv, int* controlFd, int** fds);
int getEnvironment(double* d_wait, int* t_read, int* c_read);
int strToFd(char* str);

int checkFds(int controlFd, int* fds, int fdQuantity);
int activeLoop(int controlFd, int* fds, int fdQuantity, double d_wait, int t_read, int c_read);

int emptyFd(int controlFd, int fd, int bufSize);

int main(int argc, char* argv[])
{
	int exitCode = EXIT_SUCCESS;
	
	if(argc >= 4)
	{

		int controlFd;
		int* fds;
		int fdQuantity = argc - 3;

		if(!(exitCode = (!getArgs(argc, argv, &controlFd, &fds))))
		{
			double d_wait = 0.5;
			int t_read = 3;
			int c_read = 16;

			if(!(exitCode = (!getEnvironment(&d_wait, &t_read, &c_read))))
				exitCode = !activeLoop(controlFd, fds, fdQuantity, d_wait, t_read, c_read);

			free(fds);
		}
	}
	else
	{
		fprintf(stderr, "USAGE: %s -c <control_descriptor> <pipe_descriptor1> [<pipe_descriptor2> ... <pipe_descriptorN>]\n", argv[0]);
		exitCode = EXIT_FAILURE;
	}

	return exitCode;
}

int getArgs(int argc, char** argv, int* controlFd, int** fds)
{
	int res = 1;
	int opt;

	while((opt = getopt(argc, argv, ":c:")) != -1)
	{
		switch(opt)
		{
			case 'c':
				res = ((*controlFd = strToFd(optarg)) != -1);
				break;

			case ':':
				fprintf(stderr, "ERROR: getArgs: Parameter ( %c ) needs value\n", opt);
				res = 0;
				break;

			case '?':
				fprintf(stderr, "ERROR: getArgs: Unreconized parameter ( %c )\n", optopt);
				res = 0;
				break;
		}
	}

	if(res)
	{
		if((*fds = (int*)malloc(sizeof(int) * (argc - optind))))
		{
			for(int i = 0; optind < argc; i++, optind++)
			{	
				if(!(res = (((*fds)[i] = strToFd(argv[optind])) != -1)))
					break;
			}
		}
		else
		{
			fprintf(stderr, "ERROR: getArgs: Unable to allocate file descriptors array\n");
			res = 0;
		}
	}

	return res;
}

int getEnvironment(double* d_wait, int* t_read, int* c_read)
{
	int res = 1;

	char* dwait = getenv("D_WAIT");
	char* tread = getenv("T_READ");
	char* cread = getenv("C_READ");

	int errFlag = 0;

	if(dwait)
	{
		*d_wait = strToDouble(dwait, &errFlag);
		
		if(!(res = (!errFlag)))
			fprintf(stderr, "ERROR: getEnvironment: Unable to convert D_WAIT=%s to double number\n", dwait);
	}
	
	if(res && tread)
	{
		*t_read = strToInt(tread, &errFlag);
		
		if(!(res = (!errFlag)))
			fprintf(stderr, "ERROR: getEnvironment: Unable to convert T_READ=%s to int number\n", tread);
	}

	if(res && cread)
	{
		*c_read = strToInt(cread, &errFlag);
		
		if(!(res = (!errFlag)))
			fprintf(stderr, "ERROR: getEnvironment: Unable to convert C_READ=%s to int number\n", cread);
	}

	return res;
}

int strToFd(char* str)
{
	int errFlag = 0;

	int res = strToInt(str, &errFlag);

	if(errFlag || res < 0)
	{
		fprintf(stderr, "ERROR: strToFd: ( %s ) is unable to be file descriptor number\n", str);
		res = -1;
	}

	return res;
}

int checkFds(int controlFd, int* fds, int fdQuantity)
{
	int res = 1;
	int correctFds = fdQuantity;

	if(isPipe(controlFd) && isWriteOpened(controlFd))
	{
		for(int i = 0; i < fdQuantity; i++)
		{
			if(!isPipe(fds[i] || !isReadOpened(fds[i])))
			{
				correctFds--;
				close(fds[i]);
				fds[1] = -1;
			}
		}

		if(!(res = (correctFds > 0)))
			fprintf(stderr, "ERROR: checkFds: Not enough correct descriptors\n");

	}
	else
	{
		fprintf(stderr, "ERROR: checkFds: Control descriptor is not pipe or is not opened for write\n");
		res = 0;
	}

	return res;
}

int activeLoop(int controlFd, int* fds, int fdQuantity, double d_wait, int t_read, int c_read)
{
	int res = 1;

	while(checkFds(controlFd, fds, fdQuantity))
	{

		int readsRemaining = t_read;

		for(int i = 0; i < fdQuantity; i++)
		{
			if(fds[i] == -1) continue;

			if(isPipeFull(fds[i]))
			{
				if((res = (emptyFd(controlFd, fds[i], c_read))))
					readsRemaining--;
				else
					break;
			}
			
			if(!readsRemaining) break;
		}

		if(!(res = hibernate(d_wait * DECY_UNIT)))
			break;
	}

	return res;
}

int emptyFd(int controlFd, int fd, int bufSize)
{
	int res = 1;

	char* buffer = (char*)malloc((size_t)bufSize);
	if((res = (buffer != NULL)))
	{
		int isFirstRead = 1;
		
		while(!isPipeEmpty(fd))
		{
			if(!(res = (read(fd, buffer, bufSize) != -1)))
			{
				fprintf(stderr, "ERROR: emptyFd: Unable to read from pipe\n");
				break;
			}

			if(isFirstRead)
			{
				if(!(res = (write(fd, buffer, 4) != -1)))
				{
					fprintf(stderr, "ERROR: emptyFd: Unable to write to control pipe\n");
					break;
				}
				
				isFirstRead = 0;
			}
		}
	}
	else
		fprintf(stderr, "ERROR: emptyFd: Unable to allocate buffer for pipe reading\n");

	return res;
}
