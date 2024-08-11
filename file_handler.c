
#include <stdio.h>
#include <stdlib.h>
#include "errors_handler.h"
#define MAX_LINE_LENGTH 80

FILE* OpenFile(const char *filename, const char *mode) /*open file*/
{
    FILE *file = fopen(filename, mode);
    if(file == NULL){
        exit_with_error(1, "Could not open file");
    }
    return file;
}

char* ReadLine(FILE *file) /*read line from file*/
{
    char *line = malloc(MAX_LINE_LENGTH);
    if (line == NULL){
        exit_with_error(1, "Failed to allocate memory for line");
    }
    if (fgets(line, MAX_LINE_LENGTH, file) == NULL){
        free(line);
        return NULL;
    }
    return line;
}
