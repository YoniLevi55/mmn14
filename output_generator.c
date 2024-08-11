#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "file_handler.h"
#include "string_helper.h"
#include "opcode_coding.h"
#include "errors_handler.h"
#include "symbol_table.h"
#include "data_segment.h"
#include "code_segment.h"
#include "logger.h"

void externFileMaker(char* inFile) /*creates the extern file*/
{
    int i = 0;
    int j = 0;
    int symbol_count = 0;
    FILE* extFile;
    Label** symbol_table;
    code_segment** codeSegment;

    char *fileName = malloc(strlen(inFile) + 3);
    if (fileName == NULL)
    {
        exit_with_error(EXIT_FAILURE, "Failed to allocate memory for file name.");
    }
    sprintf(fileName, "%s.ext", inFile);
    extFile = OpenFile(fileName, "w");
    symbol_table = get_symbol_table();
    symbol_count = get_symbol_count();
    codeSegment = get_code_segment();

    for (i = 0; i < symbol_count; i++) /*loops through the symbol table*/
    {
        if (strcmp(symbol_table[i]->type, ".extern") == 0) /*checks if the type is extern*/
        {
            for (j = 0; j < get_code_segment_size(); j++) /*loops through the code segment*/
            {
                if (strcmp(codeSegment[j]->name , symbol_table[i]->name)==0) /*checks if the name is the same*/
                {
                    fprintf(extFile, "%s %d\n", symbol_table[i]->name, j+100);
                }
            }
        }
    }
    fclose(extFile); /*closes the extern file*/
}
void entryFileMaker(char* inFile) /*creates the entry file*/
{
    int i=0;
    int symbol_count = 0;
    FILE* entFile;
    Label** symbol_table;

    char *fileName = malloc(strlen(inFile) + 3);
    if (fileName == NULL)
    {
        exit_with_error(EXIT_FAILURE, "Failed to allocate memory for file name.");
    }
    sprintf(fileName, "%s.ent", inFile);
    entFile = OpenFile(fileName, "w");
    symbol_table =  get_symbol_table(); /*gets the symbol table*/
    symbol_count = get_symbol_count(); /*gets the symbol count*/
    for (i = 0; i < symbol_count; i++) /*loops through the symbol table*/
    {
        if (strcmp(symbol_table[i]->type, ".entry") == 0) /*checks if the type is entry*/
        {
            logger(DEBUG, "Writing to entry file: %s %d\n", symbol_table[i]->name, symbol_table[i]->value); /*writes to the entry file*/
            fprintf(entFile, "%s %d\n", symbol_table[i]->name, symbol_table[i]->value);
        }
    }
    fclose(entFile); /*closes the entry file*/
}

void objectFileMaker(char* inFile)
{
    int i = 0;
    int counter = 0;
    int codeSegmentCounter;
    int dataSegmentCounter;
    FILE* obFile;
    unsigned short *dataSegment = get_data_segment();
    code_segment** codeSegment = get_code_segment();
    char *fileName = malloc(strlen(inFile) + 2);
    if (fileName == NULL)
    {
        exit_with_error(EXIT_FAILURE, "Failed to allocate memory for file name.");
    }
    sprintf(fileName, "%s.ob", inFile);
    obFile = OpenFile(fileName, "w");
    codeSegmentCounter = get_code_segment_size(); /*gets the code segment size*/
    dataSegmentCounter = get_data_segment_size(); /*gets the data segment size*/
    fprintf(obFile, "%d %d\n", codeSegmentCounter, dataSegmentCounter); /*prints the code segment counter and the data segment counter*/
    for (i = 0; i < codeSegmentCounter; i++) /*loops through the code segment*/
    {
        fprintf(obFile, "%04d %05o\n", counter+100, codeSegment[i]->value & 0x7FFF);
        counter++;
    }
    for (i = 0; i < dataSegmentCounter; i++) /*loops through the data segment*/
    {
        fprintf(obFile, "%04d %05o\n", counter+100, dataSegment[i] & 0x7FFF);
        counter++;
    }
    fclose(obFile); /*closes the object file*/
}
