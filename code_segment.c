#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
int* codeSegment = NULL;
int IC = 0;

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
    return sizeof(codeSegment)*sizeof(int);
}

void codeSegment_add_code(int value){
    if (codeSegment == NULL)
    {
        init_code_segment(1);
    }else{
        codeSegment = (int*)realloc(codeSegment, sizeof(int) * (IC+1));
    }
    codeSegment[IC] = value;
    IC++;
}

void clear_IC(){
    IC = 0;
}

void printCodeSegment()
{
    logger(DEBUG,"--------------------\n");
    logger(DEBUG,"Code Segment:\n");
    logger(DEBUG,"==============\n");

    for (int i = 0; i < sizeof(codeSegment); i++)
    {
        logger(DEBUG,"%d\n", codeSegment[i]);
    }
    logger(DEBUG,"--------------------\n");
}