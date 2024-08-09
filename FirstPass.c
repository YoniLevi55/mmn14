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

int IC = 0, DC = 0;
int L = 0;
// int labelCount = 0;

int *dataSegment = NULL;
int *codeSegment = NULL;


// char* opcodes[NUM_OF_OPCODES] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};

bool isLabel(char *line)
{
    char* trimmedLine = trimWhiteSpace(line);
    int colonPos = findFirstSign(trimmedLine, ':');
    bool validLabel = colonPos > 1 && ((trimmedLine[0] > 64 && trimmedLine[0] < 91) || (trimmedLine[0] > 96 && trimmedLine[0] < 123));
    if (validLabel) //checks if their is a valid label, checks that first character is a capital letter and that the colon is not the first character.
    {
        return true;
    }
    else //there is no valid label.
    {
        return false;
    }
}

char* getLabel(char* line)
{
    int colonPos = findFirstSign(line,':');
    char* label = malloc(colonPos - 1);
    strncpy(label, line, colonPos - 1);
    return label;
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

void removeLastChar(char* str) {
    int length = strlen(str);

    if (length > 0) {
        str[length - 1] = '\0';  // Set the last character to null terminator
    }
}

void firstPass(char *inFile)
{
    char *line;
    bool labelFound = false; //FLAG!
    char *fileName = malloc(strlen(inFile) + 3);
    sprintf(fileName, "%s.am", inFile);
    FILE *file = OpenFile(fileName, "r");
    char *label = NULL;
    char *operation = NULL;
    char *datatype = NULL;
    char *args = NULL;
    int lineCount = 0;
    while ((line = ReadLine(file)) != NULL)
    {
        printf("Processing line %s", line);
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
            if (labelFound && (strncmp(datatype, ".entry", 5) == 0 || strncmp(datatype, ".extern", 7) == 0))
            {
                printf("Warning: Label will be ignored.\n");
                continue;
            }

            if ((datatype != NULL) && ((strncmp(datatype, ".data", 5) == 0) || (strncmp(datatype, ".string", 7) == 0)))
            {
                if(labelFound)
                {
                    if (is_symbol_exist(label))
                    {
                        set_error(line, "Label already exists in table.");
                    }
                    else
                    {
                        add_symbol(label, DC, ".data");
                    }
                }
                if (strncmp(datatype, ".data", 5) == 0) //data coding into array
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
                    }
                }
                else if (strncmp(datatype, ".string", 7) == 0)
                {
                    for (int i = 0; i < strlen(args); i++)
                    {
                        if (dataSegment == NULL)
                        {
                            dataSegment = malloc(sizeof(int));
                        }
                        dataSegment[DC] = args[i];
                        DC++;
                        dataSegment = realloc(dataSegment, (DC) * sizeof(int)+1);
                    }
                    dataSegment[DC] = 0;
                    DC++;
                    continue;
                }
            }
            else if (strncmp(datatype, ".entry", 6) == 0 || strncmp(datatype, ".extern", 7) == 0)
            {
                if (strncmp(datatype, ".extern", 7) == 0)
                {
                    int count = 0;
                    char** outLabels = split_string(args, ' ', &count);
                    for (int i = 0; i < count; i++)
                    {
                        add_symbol(outLabels[i], 0, ".extern");
                    }
                    label = NULL;
                    operation = NULL;
                    datatype = NULL;
                    args = NULL;
                    continue;
                }
                label = NULL;
                operation = NULL;
                datatype = NULL;
                args = NULL;
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
                add_symbol(label, IC + 100, ".code");
            }
        }

        if (operation == NULL && datatype == NULL)
        {
            set_error(line, "Operation name incorrect.");
        }
        int opcodeCode = opcode_coder(operation, args);
        char* argOne = NULL;
        char* argTwo = NULL;
        int codeOne = 0;
        int codeTwo = 0;
        split_args(args, &argOne, &argTwo);
        argOne = trimWhiteSpace(argOne);
        argTwo = trimWhiteSpace(argTwo);
        if (validator(operation, argOne, argTwo) == false)
        {
            printf("Error: Invalid arguments to opcode name %s %s %s\n", operation, argOne, argTwo);

            set_error(line, "Invalid arguments to opcode name");
        }
        if (codeSegment == NULL)
        {
            // printf("Allocating memory for codeSegment\n");
            codeSegment = (int*) malloc(sizeof(int));
        }
        codeSegment[IC] = opcodeCode;
        L = getNumOfArgs(args);
        if (getNumOfArgs(args) == 2 && ((is_operand(argOne)|| is_dereferenced_operand(argOne)) && (is_operand(argTwo) || is_dereferenced_operand(argTwo))))
        {
            L = 1;
        }
        codeSegment = realloc(codeSegment, (IC + L + 1) * sizeof(int)); //error
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
                    codeSegment[IC + 1] = 0;
                    break;
                }
                codeSegment[IC + 1] = codeOne;
                break;
            case 2:
                if (findMethod(argOne)==  DIRECT)
                {
                    codeSegment[IC + 2] = codeTwo;
                    codeSegment[IC + 1] = 0;
                    break;
                }
                else if (findMethod(argTwo) == DIRECT)
                {
                    codeSegment[IC + 1] = codeOne;
                    codeSegment[IC + 2] = 0;
                    break;
                }
                else if (findMethod(argOne) == DIRECT && findMethod(argTwo) == DIRECT)
                {
                    codeSegment[IC + 2] = 0;
                    codeSegment[IC + 1] = 0;
                    break;
                }
                codeSegment[IC + 2] = codeTwo;
                codeSegment[IC + 1] = codeOne;
                break;
        }
        IC += L;
        label = NULL;
        operation = NULL;
        datatype = NULL;
        args = NULL;
        argOne = NULL;
        argTwo = NULL;
        labelFound = false;
    }
    if (get_error_count() > 0)
    {
        exit_with_error(EXIT_FAILURE, "Errors found in file." );
    }
    set_ic_offset(IC);
}

void secondPass(char *inFile)
{
    IC = 0;
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
        argOne = trimWhiteSpace(argOne);
        argTwo = trimWhiteSpace(argTwo);
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
    for (int i = 0; i < sizeof(symbol_table)/sizeof(Label); i++)
    {
        if (strcmp(symbol_table[i]->type, "entry") == 0)
        {
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

    for (int i = 0; i < sizeof(symbol_table)/sizeof(Label); i++)
    {
        if (strcmp(symbol_table[i]->type, "extern") == 0)
        {
            for (int j = 0; j < sizeof(codeSegment); j++)
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
    char *fileName = malloc(strlen(inFile) + 2);
    sprintf(fileName, "%s.ob", inFile);
    FILE* obFile = OpenFile(fileName, "w");
    int codeSegmentCounter = sizeof(codeSegment);
    int dataSegmentCounter = sizeof(dataSegment);
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