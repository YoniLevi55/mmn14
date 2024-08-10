#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
int errorCount=0;


void set_error(char* line, char* message) //set error
{
    logger(ERROR,"%s ,line: %s\n",message, line);
    errorCount++;
}

int get_error_count() //get error count
{
    return errorCount;
}

void exit_with_error(int error, char* message) //exit with error
{
    logger(ERROR,"ERROR: %s\n",message);
    exit(error);
}