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

void externFileMaker(char* inFile) //creates the extern file
{
    char *fileName = malloc(strlen(inFile) + 3);
    sprintf(fileName, "%s.ext", inFile);
    FILE* extFile = OpenFile(fileName, "w");
    Label** symbol_table = get_symbol_table();
    int symbol_count = get_symbol_count();
    code_segment** codeSegment = get_code_segment();

    for (int i = 0; i < symbol_count; i++) //loops through the symbol table
    {
        if (strcmp(symbol_table[i]->type, ".extern") == 0) //checks if the type is extern
        {
            for (int j = 0; j < get_code_segment_size(); j++) //loops through the code segment
            {
                if (strcmp(codeSegment[j]->name , symbol_table[i]->name)==0) //checks if the name is the same
                {
                    fprintf(extFile, "%s %d\n", symbol_table[i]->name, j+100);
                }
            }
        }
    }
    fclose(extFile); //closes the extern file
}
void entryFileMaker(char* inFile) //creates the entry file
{
    char *fileName = malloc(strlen(inFile) + 3);
    sprintf(fileName, "%s.ent", inFile);
    FILE* entFile = OpenFile(fileName, "w");
    Label** symbol_table =  get_symbol_table(); //gets the symbol table
    int symbol_count = get_symbol_count(); //gets the symbol count
    for (int i = 0; i < symbol_count; i++) //loops through the symbol table
    {
        if (strcmp(symbol_table[i]->type, ".entry") == 0) //checks if the type is entry
        {
            logger(DEBUG, "Writing to entry file: %s %d\n", symbol_table[i]->name, symbol_table[i]->value); //writes to the entry file
            fprintf(entFile, "%s %d\n", symbol_table[i]->name, symbol_table[i]->value);
        }
    }
    fclose(entFile); //closes the entry file
}

void objectFileMaker(char* inFile)
{
    unsigned short *dataSegment = get_data_segment();
    code_segment** codeSegment = get_code_segment();
    char *fileName = malloc(strlen(inFile) + 2);
    sprintf(fileName, "%s.ob", inFile);
    FILE* obFile = OpenFile(fileName, "w");
    int codeSegmentCounter = get_code_segment_size(); //gets the code segment size
    int dataSegmentCounter = get_data_segment_size(); //gets the data segment size
    fprintf(obFile, "%d %d\n", codeSegmentCounter, dataSegmentCounter); //prints the code segment counter and the data segment counter
    int counter = 0;
    for (int i = 0; i < codeSegmentCounter; i++) //loops through the code segment
    {
        fprintf(obFile, "%04d %05o\n", counter+100, codeSegment[i]->value & 0x7FFF);
        counter++;
    }
    for (int i = 0; i < dataSegmentCounter; i++) //loops through the data segment
    {
        fprintf(obFile, "%04d %05o\n", counter+100, dataSegment[i] & 0x7FFF);
        counter++;
    }
    fclose(obFile); //closes the object file
}
