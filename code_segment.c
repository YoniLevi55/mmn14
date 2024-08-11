#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "errors_handler.h"
#include "code_segment.h"
#include "string_helper.h"
code_segment** codeSegment = NULL;

int IC = 0;
int codeSegmentSize = 0;

/* init code segment */
void init_code_segment() 
{

    codeSegment = malloc(sizeof(code_segment*));
    if (codeSegment == NULL) 
    {
        exit_with_error(1, "Failed to allocate memory for code segment");
    }
    codeSegment[0] = (code_segment*)malloc(sizeof(code_segment));
    if (codeSegment[0] == NULL) 
    {
        exit_with_error(1, "Failed to allocate memory for code segment");
    }
}

/* get code segment */
code_segment** get_code_segment() 
{
    return codeSegment;
}

/* get IC */
int get_IC() 
{
    return IC;
}

/* get code segment size */
int get_code_segment_size() 
{
    return codeSegmentSize;
}

/* add code to code segment */
void codeSegment_add_code(unsigned short value, char* name) 
{
    if (codeSegment == NULL) 
    {
        init_code_segment();
    }
    else
    {
        codeSegment = realloc(codeSegment, sizeof(code_segment*) * (IC+1));
        codeSegment[IC] = malloc(sizeof(code_segment) );
        if (codeSegment[IC] == NULL) 
        {
            exit_with_error(1, "Failed to allocate memory for code segment");
        }
    }
    codeSegment[IC]->value = value;
    codeSegment[IC]->IC = IC + 100;
    codeSegment[IC]->name = strdup(name);
    IC++;
    codeSegmentSize++;
}




void printCodeSegment()
{
    int i = 0;
    logger(DEBUG,"--------------------\n");
    logger(DEBUG,"Code Segment:\n");
    logger(DEBUG,"==============\n");

    for (i = 0; i < codeSegmentSize; i++)
    {
        logger(DEBUG,"(%d)%d\n",i, codeSegment[i]->value);
    }
    logger(DEBUG,"--------------------\n");
}
