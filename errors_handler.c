#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
int errorCount=0;


void set_error(char* line, char* message) /* set error message*/
{
    logger(ERROR,"%s ,line: %s\n",message, line);
    errorCount++;
}

/* get error count */
int get_error_count()
{
    return errorCount;
}

void exit_with_error(int error, char* message) /* exit with error message*/
{
    logger(ERROR,"ERROR: %s\n",message);
    exit(error);
}
