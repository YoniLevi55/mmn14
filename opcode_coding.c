#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "string_helper.h"

bool isDataType(char* line)
{
    char* trimmedLine = trimWhiteSpace(line);
    if (strcmp(trimmedLine, ".data") == 0 || strcmp(trimmedLine, ".string") == 0 || strcmp(trimmedLine, ".extern") == 0 || strcmp(trimmedLine, ".entry") == 0)
    {
        return true;
    }
        return false;
}

bool isOperation(char* line)
{
    char* trimmedLine = trimWhiteSpace(line);
    char* opcodes[16] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
    for (int i = 0; i < 16; i++)
    {
        if (strcmp(trimmedLine, opcodes[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

enum OPCODES
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
    OP_UNKNOWN = -1,
};

typedef struct { char *key; int val; } t_symstruct;

t_symstruct opcodesStruct[] = {
    { "mov", OP_MOV },
    { "cmp", OP_CMP },
    { "add", OP_ADD },
    { "sub", OP_SUB },
    { "lea", OP_LEA },
    { "clr", OP_CLR },
    { "not", OP_NOT },
    { "inc", OP_INC },
    { "dec", OP_DEC },
    { "jmp", OP_JMP },
    { "bne", OP_BNE },
    { "red", OP_RED },
    { "prn", OP_PRN },
    { "jsr", OP_JSR },
    { "rts", OP_RTS },
    { "stop", OP_STOP },
    { NULL, 0 }
};

#define NKEYS (sizeof(opcodesStruct)/sizeof(t_symstruct))

enum OPCODES keyfromstring(char *key)
{
    int i;
    for (i=0; i < NKEYS; i++) {
        t_symstruct *sym = &opcodesStruct[i];
        if (strcmp(sym->key, key) == 0)
            return sym->val;
    }
    return OP_UNKNOWN;
}

enum ARE
{
    ABSOLUTE = 4,
    EXTERNAL = 1,
    RELOCATABLE = 2,
};

enum METHODS
{
    IMMEDIATE = 1,
    DIRECT = 2,
    REGISTER_INDIRECT = 4,
    REGISTER_DIRECT = 8,
};

#define OPCODE_OFFSET 11
#define ORIGIN_METHOD_OFFSET 7
#define DESTINATION_METHOD_OFFSET 3
#define ARE_OFFSET 0

bool is_number(const char* str) {
    if (*str == '-' || *str == '+') {
        str++;
    }
    if (!*str) {
        return false;
    }
    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return false;
        }
        str++;
    }
    return true;
}

bool is_number_with_hash(const char* str) {
    if (*str != '#') {
        return false;
    }
    str++;
    return is_number(str);
}

bool is_operand(const char* str) {
    return strcmp(str, "r1") == 0 || strcmp(str, "r2") == 0 || strcmp(str, "r3") == 0 ||
           strcmp(str, "r4") == 0 || strcmp(str, "r5") == 0 || strcmp(str, "r6") == 0 ||
           strcmp(str, "r7") == 0 || strcmp(str, "r8") == 0;
}

bool is_dereferenced_operand(const char* str) {
    if (*str != '*') {
        return false;
    }
    str++;
    return is_operand(str);
}

void split_args(const char* args, char* argOne, char* argTwo) {
    int count = 0;
    char** splitted = split_string(args, ',', &count);
    if (count == 1)
    {
        strcpy(argOne, splitted[0]);
    }
    else if (count == 2)
    {
        strcpy(argOne, splitted[0]);
        strcpy(argTwo, splitted[1]);
    }
}

int getNumOfArgs(const char* args) {
    int count = 0;
    char** splitted = split_string(args, ',', &count);
    return count;
}

int opcode_coder(char* opcode, char* args)
{
    int code = 0;
    int opcodeNum = keyfromstring(opcode);
    //opcode
    code |= opcodeNum << OPCODE_OFFSET;
    char *argOne, *argTwo; // maybe segmantation fault is here?
    split_args(args, argOne, argTwo);
    argOne = trimWhiteSpace(argOne);
    argTwo = trimWhiteSpace(argTwo);

    // Origin operand
    if (is_number_with_hash(argOne))
    {
        code |= IMMEDIATE << ORIGIN_METHOD_OFFSET;
    }

    else if (is_dereferenced_operand(argOne))
    {
        code |= REGISTER_INDIRECT << ORIGIN_METHOD_OFFSET;
    }
    else if (is_operand(argOne))
    {
        code |= REGISTER_DIRECT << ORIGIN_METHOD_OFFSET;
    }
    else
    {
        code |= DIRECT << ORIGIN_METHOD_OFFSET;
    }

    // Destination operand
    if (is_number_with_hash(argTwo))
    {
        code |= IMMEDIATE << DESTINATION_METHOD_OFFSET;
    }
    else if (is_dereferenced_operand(argTwo))
    {
        code |= REGISTER_INDIRECT << DESTINATION_METHOD_OFFSET;
    }
    else if (is_operand(argTwo))
    {
        code |= REGISTER_DIRECT << DESTINATION_METHOD_OFFSET;
    }
    else
    {
        code |= DIRECT << DESTINATION_METHOD_OFFSET;
    }

    code |= ABSOLUTE << ARE_OFFSET;
    return code;
}