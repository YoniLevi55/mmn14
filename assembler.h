#include <stdio.h>
#include <stdlib.h>
#include "file_handler.h"
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LABEL_LENGTH 31

typedef struct
{
    char* name;
    int value;
    char* type;
    char* attribute;
} Label;

Label** symbolTable;

int isLabel(char* line);
void firstPass(char *inFile);