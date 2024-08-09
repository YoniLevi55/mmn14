#include <stdio.h>
#include <stdlib.h>
#include "file_handler.h"
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LABEL_LENGTH 31
#define NUM_OF_OPCODES 16
#define MAX_LINE_LENGTH 81

// typedef struct
// {
//     char* name;
//     int value;
//     char* type;
// } Label;

// Label** symbolTable;

bool isLabel(char* line);
void firstPass(char *inFile);
int getLabelValue(char* label);
void breakLine(char* line, char** label, char** operation, char** datatype, char** args);
// bool isInTable(char* label);
void secondPass(char *inFile);
int intToOctal(int num);
void entryFileMaker(char* inFile);
void externFileMaker(char* inFile);
void objectFileMaker(char* inFile);
