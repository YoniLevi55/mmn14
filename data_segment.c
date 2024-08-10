#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
int* dataSegment = NULL;
int dataSegmentSize = 0;
int DC = 0;


int* get_data_segment()
{
    return dataSegment;
}

void init_data_segment(int size)
{
    dataSegment = (int*)malloc(sizeof(int) * size);
}

int get_DC()
{
    return DC;
}

int get_data_segment_size()
{
    return dataSegmentSize;
}

void dataSegment_add_data(int value)
{
    logger(DEBUG,"dataSegment:: Adding data: %d\n",value);
    if (dataSegment == NULL)
    {
        init_data_segment(1);
    } else {
        dataSegment = (int*)realloc(dataSegment, sizeof(int) * (DC+1));
    }
    dataSegment[DC] = value;
    DC++;
}