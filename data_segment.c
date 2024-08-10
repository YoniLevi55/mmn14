#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
unsigned short* dataSegment = NULL;
int dataSegmentSize = 0;
int DC = 0;


unsigned short* get_data_segment() //get data segment
{
    return dataSegment;
}

void init_data_segment(int size) //init data segment
{
    dataSegment = (unsigned short*)malloc(sizeof(unsigned short) * size);
}

int get_DC() //get DC
{
    return DC;
}

int get_data_segment_size() //get data segment size
{
    return dataSegmentSize;
}

void dataSegment_add_data(unsigned short value) //add data to data segment
{
    if (dataSegment == NULL) //if data segment is null
    {
        init_data_segment(1);
    }
    else //if data segment is not null
    {
        dataSegment = (unsigned short*)realloc(dataSegment, sizeof(unsigned short) * (DC+1));
    }
    dataSegment[DC] = value;
    DC++;
    dataSegmentSize++;
}