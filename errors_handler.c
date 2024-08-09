#include <stdio.h>
#include <stdlib.h>

int errorCount=0;


void set_error(char* line, char* message){
    printf("ERROR: %s ,line: %s\n",message, line);
    errorCount++;
}

int get_error_count(){
    return errorCount;
}

void exit_with_error(int error, char* message){
    printf("ERROR: %s\n",message);
    exit(error);
}