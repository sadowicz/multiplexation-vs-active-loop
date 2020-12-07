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
