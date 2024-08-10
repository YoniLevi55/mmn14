#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

enum ARE
{
    ABSOLUTE = 4,
    EXTERNAL = 1,
    RELOCATABLE = 2
};

enum METHODS
{
    IMMEDIATE = 1,
    DIRECT = 2,
    REGISTER_INDIRECT = 4,
    REGISTER_DIRECT = 8
};

enum OPCODES /* enum for the opcodes */
{
    OP_MOV = 0,
    OP_CMP,
    OP_ADD,
    OP_SUB,
    OP_LEA,
    OP_CLR,
    OP_NOT,
    OP_INC,
    OP_DEC,
    OP_JMP,
    OP_BNE,
    OP_RED,
    OP_PRN,
    OP_JSR,
    OP_RTS,
    OP_STOP,
    OP_UNKNOWN = -1
};

bool isDataType(char* line);
bool isOperation(char* line);
void example_pointer(char* line, char** label, char** operation, char** datatype, char** args);
enum OPCODES keyfromstring(char *key);
bool is_number(const char* str);
bool is_number_with_hash(const char* str);
bool is_operand(const char* str);
bool is_dereferenced_operand(const char* str);
void split_args(const char* args, char** argOne, char** argTwo);
unsigned short opcode_coder(char* opcode, char* args);
int getNumOfArgs(const char* args);
void operandCoder(char* argOne, char* argTwo, unsigned short *codeOne, unsigned short *codeTwo);
bool validator(char* op, char* argOne, char* argTwo);
enum METHODS findMethod(char* arg);
void labelCoder(char* arg, unsigned short *code, char* type);
