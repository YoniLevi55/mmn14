
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "file_handler.h"
#include "string_helper.h"
#include "opcode_coding.h"
#include "errors_handler.h"
#include "symbol_table.h"
#include "data_segment.h"
#include "code_segment.h"
#include "logger.h"

void secondPass(char *inFile)
{
    int i=0;
    code_segment** codeSegment = get_code_segment(); /*get code segment*/
    char *line;
    FILE *file;
    char *label = NULL;
    char *operation = NULL;
    char *datatype = NULL;
    char *args = NULL;
    char* argOne = NULL;
    char* argTwo = NULL;
    unsigned short value = 0;
    char *fileName = malloc(strlen(inFile) + 3);
    int count = 0;
    int lineCount = 0;
    if (fileName == NULL)
    {
        exit_with_error(EXIT_FAILURE, "Failed to allocate memory for file name.");
    }
    sprintf(fileName, "%s.am", inFile);
    file = OpenFile(fileName, "r");
    while ((line = ReadLine(file)) != NULL) /*reads the file line by line.*/
    {
        lineCount++;
        label = NULL;
        operation = NULL;
        datatype = NULL;
        args = NULL;
        argOne = NULL;
        argTwo = NULL;
        value = 0;
  
        breakLine(line, &label, &operation, &datatype, &args); /*breaks the line into label, operation, datatype and args.*/
        if (datatype != NULL)
        {
            bool isData = strncmp(datatype, ".data", 5) == 0;
            bool isString = strncmp(datatype, ".string", 7) == 0;
            bool isExtern = strncmp(datatype, ".extern", 7) == 0;
            bool isEntry = strncmp(datatype, ".entry", 6) == 0;
            if (isData || isString || isExtern) /*checks if the datatype is data, string or extern*/
            {
                continue;
            }
            if (isEntry) /*checks if the datatype is entry*/
            {
                char** entries = split_string(args, ' ', &count); /*splits the arguments*/
                for (i = 0; i < count; i++) /*loops through the arguments*/
                {
                    set_type(trimWhiteSpace(entries[i]), ".entry");
                }
                continue;
            }
        }
        split_args(args, &argOne, &argTwo); /*splits the arguments*/
        if (findMethod(argOne) == DIRECT && argOne != NULL) /*checks if the method is direct*/
        {
            labelCoder(argOne, &value, get_symbol(argOne)->type); /*label coding*/
            for (i = 0; i < get_code_segment_size(); i++)
            {
                if (codeSegment[i]->value == (unsigned short) -1) /*checks if the value is -1*/
                {
                    codeSegment[i]->value = value; /*sets the value*/
                    break;
                }
            }
        }
        if (findMethod(argTwo) == DIRECT && argTwo != NULL) /*checks if the method is direct*/
        {
            labelCoder(argTwo, &value, get_symbol(argTwo)->type); /*label coding*/
            for (i = 0; i < get_code_segment_size(); i++)
            {
                if (codeSegment[i]->value == (unsigned short) -1) /*checks if the value is -1*/
                {
                    codeSegment[i]->value = value; /*sets the value*/
                    break;
                }
            }
        }
    }
    if (get_error_count() > 0) /*checks if there are any errors*/
        exit_with_error(EXIT_FAILURE, "Errors found!");
}

