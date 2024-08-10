#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "file_handler.h"
#include "macro_processor.h"
#include "first_pass.h"
#include "second_pass.h"
#include "logger.h"
#include "symbol_table.h"
#include "output_generator.h"

int main(int argc, char *argv[])
{
    set_log_level(INFO);
    if (argc <= 1)
    {
        logger(INFO, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    // pre processing input files (expand macros)
    for (int i = 1; i < argc; i++)
    {
        char *filename = argv[i];
        logger(INFO, "Pre Processing file: %s\n", filename);
        preProcessFile(filename);
    }
    // run first pass on pre processes files
    for (int i = 1; i < argc; i++)
    {
        char *filename = argv[i];
        logger(INFO, "calling firstPass for file: %s\n", filename);
        firstPass(filename);
    }
    // run second pass
    for (int i = 1; i < argc; i++)
    {
        char *filename = argv[i];
        logger(INFO, "calling secondPass for file: %s\n", filename);
        secondPass(filename);
        logger(INFO, "calling entryFileMaker for file: %s\n", filename);
    }
    // create object, entry and extern files
    for (int i = 1; i < argc; i++){
        char *filename = argv[i];
        entryFileMaker(filename);
        logger(INFO, "calling externFileMaker for file: %s\n", filename);
        externFileMaker(filename);
        logger(INFO, "calling objectFileMaker for file: %s\n", filename);
        objectFileMaker(filename);
    }
    return 0;
}