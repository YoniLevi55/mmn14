#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
int* codeSegment = NULL;
int IC = 0;
int codeSegmentSize = 0;

void init_code_segment(int size){
    codeSegment = (int*)malloc(sizeof(int) * size);
}

int* get_code_segment(){
    return codeSegment;
}

int get_IC(){
    return IC;
}

int get_code_segment_size(){
    return codeSegmentSize;
}

void codeSegment_add_code(int value){
    if (codeSegment == NULL)
    {
        init_code_segment(1);
    }else{
        codeSegment = (int*)realloc(codeSegment, sizeof(int) * (IC+1));
    }
    logger(DEBUG,"codeSegment:: Adding code: %d\n",value);
    codeSegment[IC] = value;
    IC++;
    codeSegmentSize++;
}

void clear_IC(){
    IC = 0;
}

void printCodeSegment()
{
    logger(DEBUG,"--------------------\n");
    logger(DEBUG,"Code Segment:\n");
    logger(DEBUG,"==============\n");

    for (int i = 0; i < codeSegmentSize; i++)
    {
        logger(DEBUG,"%d\n", codeSegment[i]);
    }
    logger(DEBUG,"--------------------\n");
}