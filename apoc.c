#define _POSIX_C_SOURCE 2
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "utils.h"

int getArgs(int argc, char** argv, int* controlFd, int** fds);
int strToFd(char* str);

int checkFds(int controlFd, int* fds, int fdQuantity);

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
			if(!(exitCode = (!checkFds(controlFd, fds, fdQuantity))))
			{
				
			}

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
