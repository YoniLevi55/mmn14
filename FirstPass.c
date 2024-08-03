#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"
#include "FirstPass.h"
#include <stdbool.h>
#include <ctype.h>
#include "string_helper.h"
#include "opcode_coding.h"

int IC = 0, DC = 0;
int L = 0;
int *dataSegment = NULL;
int *codeSegment = NULL;
char* opcodes[NUM_OF_OPCODES] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

int isLabel(char *line)
{
    char* trimmedLine = trimWhiteSpace(line);
    int colonPos = findFirstSign(trimmedLine, ':');
    bool validLabel = colonPos > 1 && ((trimmedLine[0] > 64 && trimmedLine[0] < 91) || (trimmedLine[0] > 96 && trimmedLine[0] < 123));
    if (validLabel) //checks if their is a valid label, checks that first character is a capital letter and that the colon is not the first character.
    {
        return colonPos + 2;
    }
    else //there is no valid label.
    {
        return 0;
    }
}

char* getLabel(char* line)
{
    int colonPos = findFirstSign(line,':');
    char* label = malloc(colonPos - 1);
    strncpy(label, line, colonPos - 1);
    return label;
}

bool isInTable(char* label)
{
    for (int i = 0; i < sizeof(symbolTable); i++)
    {
        if (strcmp(symbolTable[i]->name, label) == 0)
        {
            return true;
        }
    }
    return false;
}

int getLabelValue(char* label)
{
    for (int i = 0; i < sizeof(symbolTable); i++)
    {
        if (strcmp(symbolTable[i]->name, label) == 0)
        {
            return symbolTable[i]->value;
        }
    }
    return -1;
}

void breakLine(char* line, char** label, char** operation, char** datatype, char** args) {
    int count;
    char** splitted = split_string(line, ' ', &count);
    int i=0;
    int offset = 0;
    if (isLabel(splitted[0])){
        *label = malloc(32);// change allocation size in future.
        strcpy(*label, splitted[0]);
        offset+=strlen(splitted[0]);
        i++;
    }
    if (isDataType(splitted[i]) || isOperation(splitted[i]))
    {
        if (isDataType(splitted[i]))
        {
            *datatype = malloc(32); // change allocation size in future.
            strcpy(*datatype, splitted[i]);
            offset+=strlen(splitted[i]);
        }
        else
        {
            *operation = malloc(32); // change allocation size in future.
            strcpy(*operation, splitted[i]);
            offset+=strlen(splitted[i]);
        }
        i++;
    }
    *args = malloc(strlen(line) - offset);
    strncpy(*args, &line[offset+1], strlen(line) - offset); //make sure that if data is string, return without quotes.
}

void firstPass(char *inFile)
{
    char *line;
    int labelCount = 0;
    bool labelFound = false; //FLAG!
    int errorCount = 0;
    char *fileName = malloc(strlen(inFile) + 3);
    sprintf(fileName, "%s.am", inFile);
    FILE *file = OpenFile(fileName, "r");
    char *label, *operation, *datatype, *args;
    while ((line = ReadLine(file)) != NULL)
    {
        char* trimmedLine = trimWhiteSpace(line);
        breakLine(trimmedLine, &label, &operation, &datatype, &args);
        if (trimmedLine[0] == '\n' || trimmedLine[0] == ';') //checks if the line is empty or a comment.
            continue; //ignores and skips the line if it is empty or a comment.

        if (label != NULL) //checks if the line is a label.
        {
            labelFound = true;
            labelCount++;
        }

        if ((operation != NULL) && ((strncmp(operation, ".data", 5) == 0) || (strncmp(operation, ".string", 7) == 0)))
        {
            if(labelFound)
            {
                if (labelCount == 0)
                {
                    symbolTable = malloc(sizeof(Label*));
                    symbolTable[0] = malloc(sizeof(Label));
                    symbolTable[0]->name = label;
                    symbolTable[0]->value = DC;
                    symbolTable[0]->type = ".data";
                }
                else
                {
                    if (isInTable(label))
                    {
                        fprintf(stdout, "Error: Label %s already exists in table.\n", label);
                        errorCount++;
                    }
                    else
                    {
                        symbolTable = realloc(symbolTable, sizeof(Label*) * (labelCount + 1));
                        symbolTable[labelCount] = malloc(sizeof(Label));
                        symbolTable[labelCount]->name = label;
                        symbolTable[labelCount]->value = DC;
                        symbolTable[labelCount]->type = ".data";
                    }
                }
            }
            if (strncmp(operation, ".data", 5) == 0) //data coding into array
            {
                int count = 0;
                char** data = split_string(args, ',', &count);
                for (int i = 0; i < count; i++)
                {
                    if (dataSegment == NULL)
                    {
                        dataSegment = malloc(sizeof(int));
                    }
                    else
                    {
                        dataSegment = realloc(dataSegment, (DC + 1) * sizeof(int));
                    }
                    dataSegment[DC] = atoi(data[i]);
                    DC++;
                    dataSegment = realloc(dataSegment, (DC) * sizeof(int));
                }
            }
            else if (strncmp(operation, ".string", 7) == 0)
            {
                for (int i = 0; i < strlen(args); i++)
                {
                    if (dataSegment == NULL)
                    {
                        dataSegment = malloc(sizeof(int));
                    }
                    else
                    {
                        dataSegment = realloc(dataSegment, (DC) * sizeof(int));
                    }
                    dataSegment[DC] = args[i];
                    DC++;
                    dataSegment = realloc(dataSegment, (DC) * sizeof(int));
                }
                dataSegment[DC] = 0;
                DC++;
                continue;
            }
        }
        else if (strncmp(operation, ".entry", 6) == 0 || strncmp(operation, ".extern", 7) == 0)
        {
            if (strncmp(operation, ".extern", 7) == 0)
            {
                int count = 0;
                char** outLabels = split_string(args, ' ', &count);
                for (int i = 0; i < count; i++)
                {
                    symbolTable = realloc(symbolTable, sizeof(Label*) * (labelCount + 1));
                    symbolTable[labelCount] = malloc(sizeof(Label));
                    symbolTable[labelCount]->name = outLabels[i];
                    symbolTable[labelCount]->type = ".extern";
                }
                continue;
            }
        }
        if (labelFound)
        {
            if(isInTable(label))
            {
                fprintf(stdout, "Error: Label %s already exists in table.\n", label);
                errorCount++;
            }
            else
            {
                symbolTable = realloc(symbolTable, sizeof(Label*) * (labelCount + 1));
                symbolTable[labelCount] = malloc(sizeof(Label));
                symbolTable[labelCount]->name = label;
                symbolTable[labelCount]->value = IC + 100;
                symbolTable[labelCount]->type = ".code";
            }
        }
        if (operation == NULL)
            fprintf(stdout, "Error: Operation name incorrect.\n");
        int opcodeCode = opcode_coder(operation, args);
        if (codeSegment == NULL)
        {
            codeSegment = malloc(sizeof(int));
        } else {
            codeSegment = realloc(codeSegment, (IC + 1) * sizeof(int));
        }
        codeSegment[IC] = opcodeCode;
        L = getNumOfArgs(args);
        IC += L + 1;
        codeSegment = realloc(&codeSegment, (IC) * sizeof(int));
        continue;
    }
    if (errorCount > 0)
        exit(EXIT_FAILURE);
    for (int i = 0; i < sizeof(symbolTable); i++)
    {
        if (strcmp(symbolTable[i]->type, ".data") == 0)
            symbolTable[i]->value += IC + 100;
    }
}