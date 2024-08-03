#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"
#include <stdbool.h>
#include <ctype.h>
#include "string_helper.h"
#include "FirstPass.h"
#include "opcode_coding.h"

// int IC = 100;

void secondPass(char *inFile)
{
    char *line;
    int errorCount = 0;
    char *fileName = malloc(strlen(inFile) + 3);
    sprintf(fileName, "%s.am", inFile);
    FILE *file = OpenFile(fileName, "r");
    char *label, *operation, *datatype, *args;
    while ((line = ReadLine(file)) != NULL)
    {
        // breakLine(line, &label, &operation, &datatype, &args);
        if ((strncmp(operation, ".data", 5) == 0) || (strncmp(operation, ".string", 7) == 0) || (strncmp(operation, "extern", 6) == 0))
        {
            continue;
        }
        if (strncmp(operation, ".entry", 6) == 0)
        {
            //?//
        }
        
    }
}