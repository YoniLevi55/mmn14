#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"
#include "FirstPass.h"
#include <stdbool.h>
#include <ctype.h>
#include "string_helper.h"
#include "opcode_coding.h"
#include "errors_handler.h"
#include "symbol_table.h"
#include "data_segment.h"
#include "code_segment.h"
#include "logger.h"

int L = 0;


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



void free_if_not_null(void* ptr)
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
    while ((line = ReadLine(file)) != NULL)
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

        logger(DEBUG, "Processing line (%d) %s",lineCount, line);
        if(lineCount == 20){
            logger(DEBUG, "Breakpoint\n");
        }
        lineCount++;
        char* trimmedLine = trimWhiteSpace(line);
        if (strlen(trimmedLine) > MAX_LINE_LENGTH)
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
        if (labelFound && is_symbol_exist(label))
        {
            set_error(line, "Label already defined");
        }
        if (datatype != NULL)
        {
            isData = strncmp(datatype, ".data", 5) == 0;
            isString = strncmp(datatype, ".string", 7) == 0;
            isExtern = strncmp(datatype, ".extern", 7) == 0;
            isEntry = strncmp(datatype, ".entry", 6) == 0;

            if (labelFound && (isEntry || isExtern))
            {
                logger(WARNING, "Label will be ignored.");
                continue;
            }

            if ((datatype != NULL) && (isData || isString))
            {
                if(labelFound)
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
                else if (isString)
                {
                    for (int i = 0; i < strlen(args); i++)
                    {
                        dataSegment_add_data(args[i]);
                    }
                    // dataSegment[DC] = 0;
                    // DC++;
                    continue;
                }
            }
            else if (isEntry || isExtern)
            {
                if (isExtern)
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
        if (labelFound)
        {
            if(is_symbol_exist(label))
            {
                set_error(line, "Label already exists in table.");
            }
            else
            {
                add_symbol(label, get_IC() + 100, ".code");
            }
        }

        if (operation == NULL && datatype == NULL)
        {
            set_error(line, "Operation name incorrect.");
        }
        int opcodeCode = opcode_coder(operation, args);
        int codeOne = 0;
        int codeTwo = 0;
        split_args(args, &argOne, &argTwo);
        if (validator(operation, argOne, argTwo) == false)
        {
            set_error(line, "Invalid arguments to opcode name");
        }
        codeSegment_add_code(opcodeCode);
        // if (codeSegment == NULL)
        // {
        //     // printf("Allocating memory for codeSegment\n");
        //     codeSegment = (int*) malloc(sizeof(int));
        // }
        // codeSegment[IC+1] = opcodeCode;
        L = getNumOfArgs(args);
        if (getNumOfArgs(args) == 2 && ((is_operand(argOne)|| is_dereferenced_operand(argOne)) && (is_operand(argTwo) || is_dereferenced_operand(argTwo))))
        {
            L = 1;
        }
        
        // codeSegment = realloc(codeSegment, (IC + L + 1) * sizeof(int)); //error
        operandCoder(argOne, argTwo, &codeOne, &codeTwo);
        int codeCount = 0;
        if (codeOne != 0)
        {
            codeCount++;
        }
        if (codeTwo != 0)
        {
            codeCount++;
        }
        switch (codeCount)
        {
            case 0:
                break;
            case 1:
                if (findMethod(argOne) == DIRECT)
                {
                    codeSegment_add_code(0);
                    // codeSegment[IC + 1] = 0;
                    break;
                }
                codeSegment_add_code(codeOne);
                // codeSegment[IC + 1] = codeOne;
                break;
            case 2:
                if (findMethod(argOne)==  DIRECT)
                {
                    codeSegment_add_code(0);
                    codeSegment_add_code(codeTwo);
                    // codeSegment[IC + 1] = 0;
                    // codeSegment[IC + 2] = codeTwo;
                    break;
                }
                else if (findMethod(argTwo) == DIRECT)
                {
                    codeSegment_add_code(codeOne);
                    codeSegment_add_code(0);
                    // codeSegment[IC + 1] = codeOne;
                    // codeSegment[IC + 2] = 0;
                    break;
                }
                else if (findMethod(argOne) == DIRECT && findMethod(argTwo) == DIRECT)
                {
                    codeSegment_add_code(0);
                    codeSegment_add_code(0);
                    // codeSegment[IC + 2] = 0;
                    // codeSegment[IC + 1] = 0;
                    break;
                }
                codeSegment_add_code(codeOne);
                codeSegment_add_code(codeTwo);
                // codeSegment[IC + 2] = codeTwo;
                // codeSegment[IC + 1] = codeOne;
                break;
        }
        // IC += L;
        free_if_not_null(label);
        free_if_not_null(operation);
        free_if_not_null(datatype);
        free_if_not_null(args);
        free_if_not_null(argOne);
        free_if_not_null(argTwo);
        labelFound = false;
    }
    if (get_error_count() > 0)
    {
        exit_with_error(EXIT_FAILURE, "Errors found in file." );
    }
    set_ic_offset(get_IC());
}

void secondPass(char *inFile)
{
    int IC = 0;
    int* codeSegment = get_code_segment();
    char *line;
    char *fileName = malloc(strlen(inFile) + 3);
    int count = 0;
    sprintf(fileName, "%s.am", inFile);
    FILE *file = OpenFile(fileName, "r");
    char *label, *operation, *datatype, *args;
    while ((line = ReadLine(file)) != NULL)
    {
        breakLine(line, &label, &operation, &datatype, &args);
        if ((strncmp(datatype, ".data", 5) == 0) || (strncmp(datatype, ".string", 7) == 0) || (strncmp(datatype, ".extern", 6) == 0))
        {
            continue;
        }
        if (strncmp(datatype, ".entry", 6) == 0)
        {
            char** entries = split_string(args, ' ', &count);
            for (int i = 0; i < count; i++)
            {
                set_type(entries[i], ".entry");
            }
            continue;
        }
        char* argOne = NULL;
        char* argTwo = NULL;
        split_args(args, &argOne, &argTwo);
        L = getNumOfArgs(args);
        if (getNumOfArgs(args) == 2 && ((is_operand(argOne) || is_operand(argTwo) || is_dereferenced_operand(argOne) || is_dereferenced_operand(argTwo)) || (is_operand(argOne) || is_operand(argTwo) || is_dereferenced_operand(argOne) || is_dereferenced_operand(argTwo))))
        {
            L = 1;
        }
        if (isLabel(argOne))
        {
            codeSegment[IC + 1] = getLabelValue(argOne);
        }
        if (isLabel(argTwo))
        {
            codeSegment[IC + 2] = getLabelValue(argTwo);
        }
        IC += L;
    }
    if (get_error_count() > 0)
        exit_with_error(EXIT_FAILURE, "Errors found!");
}

int intToOctal(int num) {
    int octalNum = 0, placeValue = 1;

    while (num != 0) {
        // Getting the remainder when divided by 8
        int remainder = num % 8;
        // Forming the octal number
        octalNum += remainder * placeValue;
        // Updating the place value
        placeValue *= 10;
        // Reducing the number
        num /= 8;
    }

    return octalNum;
}

void entryFileMaker(char* inFile)
{
    char *fileName = malloc(strlen(inFile) + 3);
    sprintf(fileName, "%s.ent", inFile);
    FILE* entFile = OpenFile(fileName, "w");
    Label** symbol_table =  get_symbol_table();
    int symbol_count = get_symbol_count();
    for (int i = 0; i < symbol_count; i++)
    {
        logger(DEBUG, "type: %s\n", symbol_table[i]->type);
        if (strcmp(symbol_table[i]->type, ".entry") == 0)
        {
            logger(INFO, "Writing to entry file: %s %d", symbol_table[i]->name, symbol_table[i]->value);
            fprintf(entFile, "%s %d\n", symbol_table[i]->name, symbol_table[i]->value);
        }
    }
    fclose(entFile);
}

void externFileMaker(char* inFile)
{
    char *fileName = malloc(strlen(inFile) + 3);
    sprintf(fileName, "%s.ext", inFile);
    FILE* extFile = OpenFile(fileName, "w");
    Label** symbol_table = get_symbol_table();
    int symbol_count = get_symbol_count();
    int* codeSegment = get_code_segment();

    for (int i = 0; i < symbol_count; i++)
    {
        if (strcmp(symbol_table[i]->type, "extern") == 0)
        {
            for (int j = 0; j < get_code_segment_size(); j++)
            {
                if (codeSegment[j] == symbol_table[i]->value)
                {
                    fprintf(extFile, "%s %d\n", symbol_table[i]->name, j+100);
                }
            }
        }
    }
    fclose(extFile);
}

void objectFileMaker(char* inFile)
{
    int *dataSegment = get_data_segment();
    int *codeSegment = get_code_segment();
    char *fileName = malloc(strlen(inFile) + 2);
    sprintf(fileName, "%s.ob", inFile);
    FILE* obFile = OpenFile(fileName, "w");
    int codeSegmentCounter = sizeof(codeSegment);
    int dataSegmentCounter = get_data_segment_size();
    fprintf(obFile, "%d %d\n", codeSegmentCounter, dataSegmentCounter);
    int counter = 0;
    for (int i = 0; i < codeSegmentCounter; i++)
    {
        counter++;
        fprintf(obFile, "%d %d\n", counter+100, intToOctal(codeSegment[i]));
    }
    for (int i = 0; i < dataSegmentCounter; i++)
    {
        counter++;
        fprintf(obFile, "%d %d\n", counter+100, intToOctal(dataSegment[i]));
    }
    fclose(obFile);
}
