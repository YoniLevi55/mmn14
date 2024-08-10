#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "code_segment.h"
code_segment** codeSegment = NULL;

int IC = 0;
int codeSegmentSize = 0;

void init_code_segment() //init code segment
{

    codeSegment = malloc(sizeof(code_segment*));
    codeSegment[0] = (code_segment*)malloc(sizeof(code_segment));
}

code_segment** get_code_segment() //get code segment
{
    return codeSegment;
}

int get_IC() //get IC
{
    return IC;
}

int get_code_segment_size() //get code segment size
{
    return codeSegmentSize;
}

void codeSegment_add_code(unsigned short value, char* name) //add code to code segment
{
    if (codeSegment == NULL) //if code segment is null
    {
        init_code_segment();
    }
    else //if code segment is not null
    {
        codeSegment = realloc(codeSegment, sizeof(code_segment*) * (IC+1));
        codeSegment[IC] = malloc(sizeof(code_segment) );
    }
    codeSegment[IC]->value = value;
    codeSegment[IC]->IC = IC + 100;
    codeSegment[IC]->name = strdup(name);
    IC++;
    codeSegmentSize++;
}




void printCodeSegment()
{
    logger(DEBUG,"--------------------\n");
    logger(DEBUG,"Code Segment:\n");
    logger(DEBUG,"==============\n");

    for (int i = 0; i < codeSegmentSize; i++)
    {
        logger(DEBUG,"(%d)%d\n",i, codeSegment[i]->value);
    }
    logger(DEBUG,"--------------------\n");
}