#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "file_handler.h"
#include "first_pass.h"
#include "string_helper.h"
#include "opcode_coding.h"
#include "errors_handler.h"
#include "symbol_table.h"
#include "data_segment.h"
#include "code_segment.h"
#include "logger.h"



void free_if_not_null(void* ptr) /*frees the memory if the pointer is not NULL.*/
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}

void firstPass(char *inFile)
{
    char *line;
    char* trimmedLine;
    int i=0;
    FILE *file;
    int lineCount = 0;
    int codeCount = 0;
    char *fileName = malloc(strlen(inFile) + 3);
    if (fileName == NULL)
    {
        exit_with_error(EXIT_FAILURE, "Failed to allocate memory for file name.");
    }
    sprintf(fileName, "%s.am", inFile);
    file = OpenFile(fileName, "r");

    while ((line = ReadLine(file)) != NULL) /*reads the file line by line.*/
    {
        bool labelFound = false; /*FLAG!*/
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
        unsigned short opcodeCode = 0;
        unsigned short codeOne = 0;
        unsigned short codeTwo = 0;

        lineCount++;
        trimmedLine = trimWhiteSpace(line);
        if (strlen(trimmedLine) > MAX_LINE_LENGTH) /*checks if the line is too long.*/
        {
            set_error(line, "Line is too long");
        }
        breakLine(trimmedLine, &label, &operation, &datatype, &args);
        if (trimmedLine[0] == '\n' || trimmedLine[0] == ';') /*checks if the line is empty or a comment.*/
            continue; /*ignores and skips the line if it is empty or a comment.*/
        if (label != NULL) /*checks if the line is a label.*/
        {
            labelFound = true;
            removeLastChar(label);
        }
        if (datatype != NULL)
        {
            isData = strncmp(datatype, ".data", 5) == 0;
            isString = strncmp(datatype, ".string", 7) == 0;
            isExtern = strncmp(datatype, ".extern", 7) == 0;
            isEntry = strncmp(datatype, ".entry", 6) == 0;

            if (labelFound && (isEntry || isExtern)) /*checks if the label is an entry or an extern.*/
            {
                logger(WARNING, "Label will be ignored.");
                continue;
            }
            if ((datatype != NULL) && (isData || isString)) /*checks if the label is an data or a string.*/
            {
                if(labelFound) /*checks if a label was found.*/
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
                if (isData) /*data coding into array*/
                {
                    int count = 0;
                    char** data = split_string(args, ',', &count);
                    for (i = 0; i < count; i++)
                    {
                        dataSegment_add_data(atoi(data[i]));
                    }
                }
                else if (isString) /*string coding into array*/
                {
                    args = trimWhiteSpace(args);
                    args = removeQuotes(args);
                    for (i = 0; i < strlen(args); i++)
                    {
                        dataSegment_add_data(args[i]);
                    }
                    dataSegment_add_data('\0');
                    continue;
                }
            }
            else if (isEntry || isExtern) /*entry or extern coding into array*/
            {
                if (isExtern) /*extern coding into array*/
                {
                    int count = 0;
                    char** outLabels = split_string(args, ' ', &count);
                    for (i = 0; i < count; i++)
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
        if (labelFound) /*checks if a label was found.*/
        {
            if(is_symbol_exist(label)) /*checks if the label already exists in the symbol table.*/
            {
                set_error(line, "Label already exists in table.");
            }
            else
            {
                add_symbol(label, get_IC() + 100, ".code");
            }
        }

        if (operation == NULL && datatype == NULL) /*checks if the operation name is valid.*/
        {
            set_error(line, "Operation name incorrect.");
            continue;
        }
        opcodeCode = opcode_coder(operation, args); /*opcode coding.*/
        codeOne = 0;
        codeTwo = 0;
        split_args(args, &argOne, &argTwo); /*splitting the arguments.*/
        if (validator(operation, argOne, argTwo) == false) /*checks if the arguments are valid.*/
        {
            set_error(line, "Invalid arguments to opcode name");
        }
        codeSegment_add_code(opcodeCode, operation); /*adding the opcode to the code segment.*/
        operandCoder(argOne, argTwo, &codeOne, &codeTwo); /*operand coding.*/
        codeCount = 0;
        if (codeOne != 0)
        {
            codeCount++;
        }
        if (codeTwo != 0)
        {
            codeCount++;
        }
        switch (codeCount) /*adding the operands to the code segment.*/
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
    if (get_error_count() > 0) /*checks if there are any errors.*/
    {
        exit_with_error(EXIT_FAILURE, "Errors found in file." );
    }
    set_ic_offset(get_IC()); /*sets the IC offset.*/
}

