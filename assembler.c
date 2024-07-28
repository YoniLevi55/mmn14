#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"
#include "assembler.h"
#include <stdbool.h>
#include <ctype.h>
#include "string_helper.h"

int IC = 0, DC = 0;

int findFirstColon(char *line)
{
    char *pos = strchr(line, ':');
    if (pos)
    {
        return pos - line;
    }
    else
    {
        return -1;
    }
}

int isLabel(char *line)
{
    int colonPos = findFirstColon(line);
    bool validLabel = colonPos > 1 && ((line[0] > 64 && line[0] < 91) || (line[0] > 96 && line[0] < 123));
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
    int colonPos = findFirstColon(line);
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



void firstPass(char *inFile)
{
    char *line;
    int labelCount = 0;
    bool labelFound = false;
    int labelOffSet = 0;
    int errorCount = 0;
    char *fileName = malloc(strlen(inFile) + 3);
    sprintf(fileName, "%s.am", inFile);
    FILE *file = OpenFile(fileName, "r");
    while ((line = ReadLine(file)) != NULL)
    {
        char* trimmedLine = trimWhiteSpace(line);
        if (trimmedLine[0] == '\n' || trimmedLine[0] == ';') //checks if the line is empty or a comment.
            continue; //ignores and skips the line if it is empty or a comment.
        labelOffSet = isLabel(line);
        if (labelOffSet > 0) //checks if the line is a label.
        {
            labelFound = true;
            labelCount++;
        }
        if(strncmp(&line[labelOffSet], ".data", 5) == 0 || strncmp(&line[labelOffSet], ".string", 7) == 0)
        {
            if(labelFound)
            {
                if (labelCount == 0)
                {
                    symbolTable = malloc(sizeof(Label*));
                    symbolTable[0] = malloc(sizeof(Label));
                    symbolTable[0]->name = getLabel(line);
                    symbolTable[0]->value = DC;
                    symbolTable[0]->type = ".data";
                }
                else
                {
                    if (isInTable(getLabel(line)))
                    {
                        fprintf(stderr, "Error: Label %s already exists in table.\n", getLabel(line));
                        errorCount++;
                    }
                    else
                    {
                        symbolTable = realloc(symbolTable, sizeof(Label*) * (labelCount + 1));
                        symbolTable[labelCount] = malloc(sizeof(Label));
                        symbolTable[labelCount]->name = getLabel(line);
                        symbolTable[labelCount]->value = DC;
                        symbolTable[labelCount]->type = ".data";
                    }
                }
            }
        }
        else if (strncmp(&line[labelOffSet], ".entry", 6) == 0 || strncmp(&line[labelOffSet], ".extern", 7) == 0)
        {
            if (strncmp(&line[labelOffSet], ".extern", 7) == 0)
            {
            }
        }
    }
}



