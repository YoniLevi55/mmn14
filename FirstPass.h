#include <stdio.h>
#include <stdlib.h>
#include "file_handler.h"
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


#define MAX_LABEL_LENGTH 31
#define NUM_OF_OPCODES 16

typedef struct
{
    char* name;
    int value;
    char* type;
} Label;

Label** symbolTable;

int isLabel(char* line);
void firstPass(char *inFile);