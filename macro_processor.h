#include <stdio.h>
#include <stdlib.h>
#include "file_handler.h"
#include <string.h>
#include <stdbool.h>


#define MAX_MACRO_LENGTH 80

typedef struct
{
    char* Name;
    char** Body;
    int LineCount;
} Macro;

Macro** macros;

void preProcessFile(char* inputFile);
Macro* macroExists(char* line);
bool isMacro(char* line);
