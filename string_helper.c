#include <stdio.h>

char* trimWhiteSpace(char* line) //trimming white space from the beginning of the line.
{
    int i = 0;
    while (line[i] == ' ' || line[i] == '\t')
        i++;
    return line + i;
}