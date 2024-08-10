#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
int errorCount=0;


void set_error(char* line, char* message){
    logger(ERROR,"%s ,line: %s\n",message, line);
    errorCount++;
}

int get_error_count(){
    return errorCount;
}

void exit_with_error(int error, char* message){
    logger(ERROR,"ERROR: %s\n",message);
    exit(error);
}