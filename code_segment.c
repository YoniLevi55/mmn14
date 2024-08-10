#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "code_segment.h"
code_segment** codeSegment = NULL;

int IC = 0;
int codeSegmentSize = 0;

void init_code_segment(){

    codeSegment = malloc(sizeof(code_segment*));
    codeSegment[0] = (code_segment*)malloc(sizeof(code_segment));
}

code_segment** get_code_segment(){
    return codeSegment;
}

int get_IC(){
    return IC;
}

int get_code_segment_size(){
    return codeSegmentSize;
}

void codeSegment_add_code(int value, char* name){
    if (codeSegment == NULL)
    {
        init_code_segment();
    }else{
        codeSegment = realloc(codeSegment, sizeof(code_segment*) * (IC+1));
        codeSegment[IC] = malloc(sizeof(code_segment) );
    }

    logger(DEBUG,"codeSegment:: Adding code: %d\n",value);
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