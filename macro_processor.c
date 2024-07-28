#include <stdio.h>
#include <stdlib.h>
#include "file_handler.h"
#include <string.h>
#include "macro_processor.h"
#include <stdbool.h>

int macroCount = 0;

char* trimWhiteSpace(char* line) //trimming white space from the beginning of the line.
{
    int i = 0;
    while (line[i] == ' ' || line[i] == '\t')
        i++;
    return line + i;
}

bool isMacro(char* line) //checking if the line is a macro.
{
    char* trimmedLine = trimWhiteSpace(line);
    return strncmp(trimmedLine, "macr", 4) == 0;
}

Macro* macroExists(char* line) //checking if the macro exists already in the macros array.
{
    char* trimmedLine = trimWhiteSpace(line);
    for (int i = 0; i < macroCount; i++)
        if (strncmp(trimmedLine, macros[i]->Name, strlen(macros[i]->Name)) == 0)
            return macros[i]; //returning the macro if it exists.
    return NULL; //returning NULL if the macro does not exist.
}

void processFile(char* inputFile, char* outputFile) //processing the initial input file.
{
    bool macroFound = false;
    FILE* inFile = OpenFile(inputFile, "r"); //opening the input file for reading.
    FILE* outFile = OpenFile(outputFile, "w"); //creating the output file for writing.
    char* line;
    int i = 0;
    Macro* currentMacro; //creating a pointer to the current macro.
    while ((line = ReadLine(inFile)) != NULL)
    {
        if (isMacro(line)) //checking if the line is a macro.
        {
            if (macroCount == 0) //allocating memory for the macros array.
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
        else if (macroFound) //checking if the macro is found.
        {
            if (strncmp(line, "endmacr", 7) == 0) //checking if the end of the macro is found.
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
        else if((currentMacro = macroExists(line)) != NULL) //checking if the macro exists in the database already.
        {
            printf("Macro found: %s\n", currentMacro->Name);
            printf("Expanding macro...\n");
            printf("Writing to output file...\n");
            printf("Macro size: %d\n", currentMacro->LineCount);
            for (int i = 0; i < currentMacro->LineCount; i++) //printing the macro to the output file.
                fprintf(outFile, "%s", currentMacro->Body[i]);
        }
        else
        {
            fprintf(outFile, "%s", line); //printing the line to the output file (if it is not a macro).
        }
    }
    fclose(inFile); //closing the input file.
    fclose(outFile); //closing the output file.
}