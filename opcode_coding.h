#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "string_helper.h"
#include "opcode_coding.c"

bool isDataType(char* line);
bool isOperation(char* line);
void example_pointer(char* line, char** label, char** operation, char** datatype, char** args);
enum OPCODES keyfromstring(char *key);
bool is_number(const char* str);
bool is_number_with_hash(const char* str);
bool is_operand(const char* str);
bool is_dereferenced_operand(const char* str);
void split_args(const char* args, char* argOne, char* argTwo);

