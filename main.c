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
    if (argc <= 1)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    for (int i = 1; i < argc; i++)
    {
        char *filename = argv[i];
        printf("Pre Processing file: %s\n", filename);
        // 1. preprocess file
        preProcessFile(filename);
        // 2. first pass
        // 3. second pass
        // 4. output files

    }
    return 0;
}
