#include <stdio.h>
#include <stdlib.h>
#include "file_handler.h"
#include <string.h>
#include "macro_processor.h"
#include <stdbool.h>
#include "string_helper.h"
#include "first_pass.h"
#include "logger.h"
#include "errors_handler.h"
#include "opcode_coding.h"
char *symbolNames = NULL;
int macroCount = 0;

bool isMacro(char* line) /*checking if the line is a macro.*/
{
    char* trimmedLine = trimWhiteSpace(line);
    return strncmp(trimmedLine, "macr", 4) == 0;
}

Macro* macroExists(char* line) /*checking if the macro exists already in the macros array.*/
{
    int i;
    char* trimmedLine = trimWhiteSpace(line);
    for (i = 0; i < macroCount; i++)
        if (strncmp(trimmedLine, macros[i]->Name, strlen(macros[i]->Name)) == 0)
            return macros[i]; /*returning the macro if it exists.*/
    return NULL; /*returning NULL if the macro does not exist.*/
}

void preProcessFile(char* inputFile) /*processing the initial input file.*/
{
    int j = 0;
    int i = 0;
    FILE* inFile;
    FILE* outFile;
    char* line;
    int labels = 0;
    int errorCount = 0;
    Macro* currentMacro; /*creating a pointer to the current macro.*/


    bool macroFound = false; /*FLAG!*/
    char *fileName = malloc(strlen(inputFile) + 3);
    char *outputFile = malloc(strlen(inputFile) + 3);
    sprintf(outputFile, "%s.am", inputFile);
    sprintf(fileName, "%s.as", inputFile);
    inFile = OpenFile(fileName, "r"); /*opening the input file for reading.*/
    outFile = OpenFile(outputFile, "w"); /*creating the output file for writing.*/
    
    while ((line = ReadLine(inFile)) != NULL)
    {
        line = trimWhiteSpace(line); /*trimming white space from the beginning of the line.*/
        if (isLabel(line))
        {
            int count = 0;
            char** labelSplit = split_string(line, ':', &count);
            if (symbolNames == NULL)
            {
                symbolNames = malloc(strlen(labelSplit[0]) + 1);
                strcpy(&symbolNames[labels], labelSplit[0]);
                labels++;
            }
            else
            {
                symbolNames = realloc(symbolNames, strlen(symbolNames) + strlen(labelSplit[0]) + 1);
                strcpy(&symbolNames[labels], labelSplit[0]);
                labels++;
            }
        }
        if (isMacro(line)) /*checking if the line is a macro.*/
        {
            if (macroCount == 0) /*allocating memory for the macros array.*/
            {
                macros = malloc(sizeof(Macro*));
                macros[0] = malloc(sizeof(Macro));
                macros[0]->LineCount = 0;
            }
            else
            {
                macros = realloc(macros, sizeof(Macro*)*(macroCount + 1));
                macros[macroCount] = malloc(sizeof(Macro));
            }
            macroCount++;
            macroFound  = true;
            macros[macroCount - 1]->Name = malloc(strlen(line) - 5);
            if (line[strlen(line) - 1] == '\n')
                strncpy(macros[macroCount - 1]->Name, line + 5, strlen(line) - 6);
            else
                strncpy(macros[macroCount - 1]->Name, line + 5, strlen(line) - 5);
        }
        else if (macroFound) /*checking if the macro is found.*/
        {
            if (strncmp(line, "endmacr", 7) == 0) /*checking if the end of the macro is found.*/
            {
                macroFound = 0;
                i = 0;
            }
            else
            {
                if (i == 0)
                    macros[macroCount - 1]->Body = malloc(sizeof(char*) );
                else
                    macros[macroCount - 1]->Body = realloc(macros[macroCount - 1]->Body, (i+1) * sizeof(char*) );
                macros[macroCount - 1]->Body[i] = malloc(strlen(line));
                strncpy( macros[macroCount - 1]->Body[i], line, strlen(line));
                macros[macroCount - 1]->LineCount +=1;
                i++;
            }
        }
        else if((currentMacro = macroExists(line)) != NULL) /*checking if the macro exists in the database already.*/
        {
            logger(DEBUG, "Macro found: %s\n", currentMacro->Name);
            logger(DEBUG, "Expanding macro...\n");
            logger(DEBUG, "Writing to output file...\n");
            logger(DEBUG, "Macro size: %d\n", currentMacro->LineCount);
            for (i = 0; i < currentMacro->LineCount; i++) /*printing the macro to the output file.*/
                fprintf(outFile, "%s", currentMacro->Body[i]);
        }
        else
        {
            fprintf(outFile, "%s\n", line); /*printing the line to the output file (if it is not a macro).*/
        }
    }
    for (i = 0; i < macroCount; i++) /*freeing the memory allocated for the macros.*/
    {
        for (j = 0; j < labels; j++)
        {
            if (macros[i]->Name == &symbolNames[j])
            {
                logger(ERROR, "Error: Macro name conflicts with label name.\n");
                errorCount++;
            }
            if(isOperation(macros[i]->Name))
            {
                logger(ERROR, "Error: Macro name conflicts with operation name.\n");
                errorCount++;
            }
        }
    }
    if (errorCount > 0)
    {
        exit_with_error(1, "Errors found. Exiting...\n");
    }
    fclose(inFile); /*closing the input file.*/
    fclose(outFile); /*closing the output file.*/
}
