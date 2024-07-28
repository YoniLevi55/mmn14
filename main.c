#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"
#include "labels.h"
#include "macro_processor.h"
#include "assembler.h"
#include <stdbool.h>

// typedef struct {
//     char label[MAX_LABEL_LENGTH];
//     int lineNumber;
// } Label;

int main(int argc, char *argv[])
{
    int labelCount = 0;
    Label labels[MAX_LINE_LENGTH];
    int lineNumber = 0;

    FILE* file = OpenFile("in.as", "r");
    char *line;
    while ((line = ReadLine(file)) != NULL) {
        lineNumber++;
        printf("%s", line);
        char label[MAX_LABEL_LENGTH];

        if (isLabel(line, label)) {
            strcpy(labels[labelCount].label, label);
            labels[labelCount].lineNumber = lineNumber;
            labelCount++;
        }
    }
    processFile("in.as", "out.as");
    return 0;
}
