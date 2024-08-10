#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_handler.h"
#include "macro_processor.h"
#include "FirstPass.h"
#include <stdbool.h>
#include "logger.h"
#include "symbol_table.h"

int main(int argc, char *argv[])
{
    set_log_level(DEBUG);
    if (argc <= 1)
    {
        logger(INFO, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    //TODO: need to loop all files in first pass and only then loop all files again in second pass
    for (int i = 1; i < argc; i++)
    {
        char *filename = argv[i];
        logger(INFO, "Processing file: %s\n", filename);
        preProcessFile(filename);
        logger(INFO, "calling firstPass for file: %s\n", filename);
        firstPass(filename);
        printCodeSegment();
        print_symbol_table();
        logger(INFO, "calling secondPass for file: %s\n", filename);
        secondPass(filename);
        printCodeSegment();
        print_symbol_table();
        logger(INFO, "calling entryFileMaker for file: %s\n", filename);
        entryFileMaker(filename);
        logger(INFO, "calling externFileMaker for file: %s\n", filename);
        externFileMaker(filename);
        logger(INFO, "calling objectFileMaker for file: %s\n", filename);
        objectFileMaker(filename);
    }
    return 0;
}