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
        unsigned short opcodeCode = opcode_coder(operation, args);
        unsigned short codeOne = 0;
        unsigned short codeTwo = 0;
        split_args(args, &argOne, &argTwo);
        if (validator(operation, argOne, argTwo) == false)
        {
            set_error(line, "Invalid arguments to opcode name");
        }
        codeSegment_add_code(opcodeCode, operation);
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
    if (get_error_count() > 0)
    {
        exit_with_error(EXIT_FAILURE, "Errors found in file." );
    }
    set_ic_offset(get_IC());
}

void secondPass(char *inFile)
{
    int IC = 0;
    code_segment** codeSegment = get_code_segment();
    char *line;
    char *fileName = malloc(strlen(inFile) + 3);
    int count = 0;
    int lineCount = 0;
    sprintf(fileName, "%s.am", inFile);
    FILE *file = OpenFile(fileName, "r");
    while ((line = ReadLine(file)) != NULL)
    {
        if (lineCount == 3)
        {
            logger(DEBUG, "Breakpoint\n");
        }
        logger(DEBUG, "Processing line (%d): %s",lineCount, line);
        lineCount++;
        char *label = NULL;
        char *operation = NULL;
        char *datatype = NULL;
        char *args = NULL;
        unsigned short value = 0;
        breakLine(line, &label, &operation, &datatype, &args);
        if (datatype != NULL)
        {
            bool isData = strncmp(datatype, ".data", 5) == 0;
            bool isString = strncmp(datatype, ".string", 7) == 0;
            bool isExtern = strncmp(datatype, ".extern", 7) == 0;
            bool isEntry = strncmp(datatype, ".entry", 6) == 0;
            if (isData || isString || isExtern)
            {
                continue;
            }
            if (isEntry)
            {
                char** entries = split_string(args, ' ', &count);
                for (int i = 0; i < count; i++)
                {
                    set_type(trimWhiteSpace(entries[i]), ".entry");
                }
                continue;
            }
        }
        char* argOne = NULL;
        char* argTwo = NULL;
        split_args(args, &argOne, &argTwo);
        if (findMethod(argOne) == DIRECT && argOne != NULL)
        {
            labelCoder(argOne, &value, get_symbol(argOne)->type);
            for (int i = 0; i < get_code_segment_size(); i++)
            {
                if (codeSegment[i]->value == (unsigned short) -1)
                {
                    codeSegment[i]->value = value;
                    break;
                }
            }
        }
        if (findMethod(argTwo) == DIRECT && argTwo != NULL)
        {
            labelCoder(argTwo, &value, get_symbol(argTwo)->type);
            for (int i = 0; i < get_code_segment_size(); i++)
            {
                if (codeSegment[i]->value == (unsigned short)-1)
                {
                    codeSegment[i]->value = value;
                    break;
                }
            }
        }
    }
    if (get_error_count() > 0)
        exit_with_error(EXIT_FAILURE, "Errors found!");
}

unsigned short intToOctal(unsigned short num) {
    unsigned short octalNum = 0, placeValue = 1;

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
    code_segment** codeSegment = get_code_segment();

    for (int i = 0; i < symbol_count; i++)
    {
        if (strcmp(symbol_table[i]->type, ".extern") == 0)
        {
            for (int j = 0; j < get_code_segment_size(); j++)
            {
                if (strcmp(codeSegment[j]->name , symbol_table[i]->name)==0)
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
    unsigned short *dataSegment = get_data_segment();
    code_segment** codeSegment = get_code_segment();
    char *fileName = malloc(strlen(inFile) + 2);
    sprintf(fileName, "%s.ob", inFile);
    FILE* obFile = OpenFile(fileName, "w");
    int codeSegmentCounter = get_code_segment_size();
    int dataSegmentCounter = get_data_segment_size();
    fprintf(obFile, "%d %d\n", codeSegmentCounter, dataSegmentCounter);
    int counter = 0;
    for (int i = 0; i < codeSegmentCounter; i++)
    {
        fprintf(obFile, "%d %05o\n", counter+100, codeSegment[i]->value & 0x7FFF);
        counter++;
    }
    for (int i = 0; i < dataSegmentCounter; i++)
    {
        fprintf(obFile, "%d %05o\n", counter+100, dataSegment[i] & 0x7FFF);
        counter++;
    }
    fclose(obFile);
}
