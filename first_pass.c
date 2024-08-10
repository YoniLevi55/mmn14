#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"
#include "first_pass.h"
#include <stdbool.h>
#include <ctype.h>
#include "string_helper.h"
#include "opcode_coding.h"
#include "errors_handler.h"
#include "symbol_table.h"
#include "data_segment.h"
#include "code_segment.h"
#include "logger.h"



void free_if_not_null(void* ptr) //frees the memory if the pointer is not NULL.
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}

void firstPass(char *inFile)
{
    char *line;
    char *fileName = malloc(strlen(inFile) + 3);
    sprintf(fileName, "%s.am", inFile);
    FILE *file = OpenFile(fileName, "r");

    int lineCount = 0;
    while ((line = ReadLine(file)) != NULL) //reads the file line by line.
    {
        bool labelFound = false; //FLAG!
        bool isEntry = false;
        bool isExtern = false;
        bool isData = false;
        bool isString = false;
        char *label = NULL;
        char *operation = NULL;
        char *datatype = NULL;
        char *args = NULL;
        char* argOne = NULL;
        char* argTwo = NULL;

        lineCount++;
        char* trimmedLine = trimWhiteSpace(line);
        if (strlen(trimmedLine) > MAX_LINE_LENGTH) //checks if the line is too long.
        {
            set_error(line, "Line is too long");
        }
        breakLine(trimmedLine, &label, &operation, &datatype, &args);
        if (trimmedLine[0] == '\n' || trimmedLine[0] == ';') //checks if the line is empty or a comment.
            continue; //ignores and skips the line if it is empty or a comment.
        if (label != NULL) //checks if the line is a label.
        {
            labelFound = true;
            removeLastChar(label);
        }
        if (labelFound && is_symbol_exist(label)) //checks if the label already exists in the symbol table.
        {
            set_error(line, "Label already defined");
        }
        if (datatype != NULL)
        {
            isData = strncmp(datatype, ".data", 5) == 0;
            isString = strncmp(datatype, ".string", 7) == 0;
            isExtern = strncmp(datatype, ".extern", 7) == 0;
            isEntry = strncmp(datatype, ".entry", 6) == 0;

            if (labelFound && (isEntry || isExtern)) //checks if the label is an entry or an extern.
            {
                logger(WARNING, "Label will be ignored.");
                continue;
            }
            if ((datatype != NULL) && (isData || isString)) //checks if the label is an data or a string.
            {
                if(labelFound) //checks if a label was found.
                {
                    if (is_symbol_exist(label))
                    {
                        set_error(line, "Label already exists in table.");
                    }
                    else
                    {
                        add_symbol(label, get_DC(), ".data");
                    }
                }
                if (isData) //data coding into array
                {
                    int count = 0;
                    char** data = split_string(args, ',', &count);
                    for (int i = 0; i < count; i++)
                    {
                        dataSegment_add_data(atoi(data[i]));
                    }
                }
                else if (isString) //string coding into array
                {
                    args = trimWhiteSpace(args);
                    args = removeQuotes(args);
                    for (int i = 0; i < strlen(args); i++)
                    {
                        dataSegment_add_data(args[i]);
                    }
                    dataSegment_add_data('\0');
                    continue;
                }
            }
            else if (isEntry || isExtern) //entry or extern coding into array
            {
                if (isExtern) //extern coding into array
                {
                    int count = 0;
                    char** outLabels = split_string(args, ' ', &count);
                    for (int i = 0; i < count; i++)
                    {
                        add_symbol(outLabels[i], 0, ".extern");
                    }
                    free_if_not_null(label);
                    free_if_not_null(operation);
                    free_if_not_null(datatype);
                    free_if_not_null(args);
                    free_if_not_null(argOne);
                    free_if_not_null(argTwo);
                    continue;
                }
                free_if_not_null(label);
                free_if_not_null(operation);
                free_if_not_null(datatype);
                free_if_not_null(args);
                free_if_not_null(argOne);
                free_if_not_null(argTwo);

                continue;
            }
            continue;
        }
        if (labelFound) //checks if a label was found.
        {
            if(is_symbol_exist(label)) //checks if the label already exists in the symbol table.
            {
                set_error(line, "Label already exists in table.");
            }
            else
            {
                add_symbol(label, get_IC() + 100, ".code");
            }
        }

        if (operation == NULL && datatype == NULL) //checks if the operation name is valid.
        {
            set_error(line, "Operation name incorrect.");
        }
        unsigned short opcodeCode = opcode_coder(operation, args); //opcode coding.
        unsigned short codeOne = 0;
        unsigned short codeTwo = 0;
        split_args(args, &argOne, &argTwo); //splitting the arguments.
        if (validator(operation, argOne, argTwo) == false) //checks if the arguments are valid.
        {
            set_error(line, "Invalid arguments to opcode name");
        }
        codeSegment_add_code(opcodeCode, operation); //adding the opcode to the code segment.
        operandCoder(argOne, argTwo, &codeOne, &codeTwo); //operand coding.
        int codeCount = 0;
        if (codeOne != 0)
        {
            codeCount++;
        }
        if (codeTwo != 0)
        {
            codeCount++;
        }
        switch (codeCount) //adding the operands to the code segment.
        {
            case 0:
                break;
            case 1:
                if (findMethod(argOne) == DIRECT)
                {
                    codeSegment_add_code(-1, argOne);
                    break;
                }
                codeSegment_add_code(codeOne, argOne);
                break;
            case 2:
                if (findMethod(argOne) == DIRECT && findMethod(argTwo) != DIRECT)
                {
                    codeSegment_add_code(-1, argOne);
                    codeSegment_add_code(codeTwo, argTwo);
                    break;
                }
                else if (findMethod(argTwo) == DIRECT && findMethod(argOne) != DIRECT)
                {
                    codeSegment_add_code(codeOne, argOne);
                    codeSegment_add_code(-1, argTwo);
                    break;
                }
                else if (findMethod(argOne) == DIRECT && findMethod(argTwo) == DIRECT)
                {
                    codeSegment_add_code(-1, argOne);
                    codeSegment_add_code(-1, argTwo);
                    break;
                }
                codeSegment_add_code(codeOne, argOne);
                codeSegment_add_code(codeTwo, argTwo);
                break;
        }
        free_if_not_null(label);
        free_if_not_null(operation);
        free_if_not_null(datatype);
        free_if_not_null(args);
        free_if_not_null(argOne);
        free_if_not_null(argTwo);
        labelFound = false;
    }
    if (get_error_count() > 0) //checks if there are any errors.
    {
        exit_with_error(EXIT_FAILURE, "Errors found in file." );
    }
    set_ic_offset(get_IC()); //sets the IC offset.
}


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
        fprintf(obFile, "%d %05o\n", counter+100, codeSegment[i]->value & 0x7FFF);
        counter++;
    }
    for (int i = 0; i < dataSegmentCounter; i++) //loops through the data segment
    {
        fprintf(obFile, "%d %05o\n", counter+100, dataSegment[i] & 0x7FFF);
        counter++;
    }
    fclose(obFile); //closes the object file
}
