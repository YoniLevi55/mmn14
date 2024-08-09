#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "string_helper.h"
#include "FirstPass.h"

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

t_symstruct opcodes[] = {
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
    { NULL, OP_UNKNOWN }
};

#define NKEYS (sizeof(opcodes)/sizeof(t_symstruct)-1)

enum OPCODES keyfromstring(char *key)
{
    // printf("key: %s\n", key);
    if (key == NULL){
        return OP_UNKNOWN;
    }
    int i;
    for (i=0; i < NKEYS; i++) {
        t_symstruct *sym = &opcodes[i];
        if (strcmp(sym->key, key) == 0){
            return sym->val;
        }
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

bool isOperation(char* line)
{
    char* trimmedLine = trimWhiteSpace(line);

    return (keyfromstring(trimmedLine)!= OP_UNKNOWN);
}

bool isDataType(char* line)
{
    char* trimmedLine = trimWhiteSpace(line);
    if (strcmp(trimmedLine, ".data") == 0 || strcmp(trimmedLine, ".string") == 0 || strcmp(trimmedLine, ".extern") == 0 || strcmp(trimmedLine, ".entry") == 0)
    {
        return true;
    }
        return false;
}

bool is_number(const char* str)
{
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

bool is_number_with_hash(const char* str)
{
    if (str == NULL) {
        return false;
    }
    else if (*str != '#') {
        return false;
    }
    str++;
    return is_number(str);
}

bool is_operand(const char* str)
{
   if ((str != NULL) && (strcmp(str, "r1") == 0 || strcmp(str, "r2") == 0 || strcmp(str, "r3") == 0 || strcmp(str, "r4") == 0 || strcmp(str, "r5") == 0 || strcmp(str, "r6") == 0 || strcmp(str, "r7") == 0 || strcmp(str, "r0") == 0))
   {
        return true;
   }
   return false;
}

bool is_dereferenced_operand(const char* str)
{
    if (str == NULL) {
        return false;
    }
    else if (*str != '*') {
        return false;
    }
    str++;
    return is_operand(str);
}

void split_args(const char* args, char** argOne, char** argTwo) {
    if (args == NULL) {
        *argOne = NULL;
        *argTwo = NULL;
        return;
    }
    int count = 0;
    char** splitted = split_string(args, ',', &count);
    if (count == 1)
    {
        *argOne = malloc(strlen(splitted[0]));
        strcpy(*argOne, splitted[0]);
    }
    else if (count == 2)
    {
        // printf("argOne: %s\n", splitted[0]);
        // printf("argTwo: %s\n", splitted[1]);
        *argOne = malloc(strlen(splitted[0]));
        *argTwo = malloc(strlen(splitted[1]));
        strcpy(*argOne, splitted[0]);
        strcpy(*argTwo, splitted[1]);
    }
}

int getNumOfArgs(const char* args)
{
    int count = 0;
    char** splitted = split_string(args, ',', &count);
    return count;
}

int findMethod(char* arg)
{
    if (is_number_with_hash(arg))
    {
        return 1;
    }
    else if (is_dereferenced_operand(arg))
    {
        return 3;
    }
    else if (is_operand(arg))
    {
        return 4;
    }
    else
    {
        return 2;
    }
}

void operandCoder(char* argOne, char* argTwo, int *codeOne, int *codeTwo)
{
    argOne = trimWhiteSpace(argOne);
    argTwo = trimWhiteSpace(argTwo);
    if (findMethod(argOne) == 1)
    {
        int value = atoi(&argOne[1]);
        *codeOne = (ABSOLUTE | (value << 6));
    }
    if (findMethod(argTwo) == 1)
    {
        int value = atoi(&argTwo[1]);
        *codeTwo = (ABSOLUTE | (value << 6));
    }
    if (findMethod(argOne) == 2)
    {
        *codeOne = -1;
    }
    if (findMethod(argTwo) == 2)
    {
        *codeTwo = -1;
    }
    if (findMethod(argOne) == 4  && argTwo == NULL)
    {
        int value = atoi(&argOne[1]);
        *codeOne = (ABSOLUTE | (value << 3));
    }
    else if (findMethod(argOne) == 4 && (findMethod(argTwo) == 1 || findMethod(argTwo) == 2))
    {
        int value = atoi(&argOne[1]);
        *codeOne = (ABSOLUTE | (value << 6));
    }
    else if (findMethod(argTwo) == 4 && (findMethod(argOne) == 1|| findMethod(argOne) == 2))
    {
        int value = atoi(&argTwo[1]);
        *codeTwo = (ABSOLUTE | (value << 3));
    }
    else if (findMethod(argOne) == 3 && argTwo == NULL)
    {
        int value = atoi(&argOne[2]);
        *codeOne = (ABSOLUTE | (value << 3));
    }
    else if (findMethod(argOne) == 3&& (findMethod(argTwo) == 1|| findMethod(argTwo) == 2))
    {
        int value = atoi(&argOne[2]);
        *codeOne = (ABSOLUTE | (value << 6));
    }
    else if (findMethod(argTwo) == 3 && (findMethod(argOne) == 1|| findMethod(argOne) == 2))
    {
        int value = atoi(&argTwo[2]);
        *codeTwo = (ABSOLUTE | (value << 3));
    }
    else if ((findMethod(argOne) == 4 || findMethod(argOne) == 3) && (findMethod(argTwo) == 4 || findMethod(argTwo) == 3))
    {
        int value = 0;
        int value2  = 0;
        if (findMethod(argOne) == 4)
        {
            value = atoi(&argOne[1]);
        }
        else if (findMethod(argOne) == 3)
        {
            value = atoi(&argOne[2]);
        }
        if (findMethod(argTwo) == 4)
        {
            value2 = atoi(&argTwo[1]);
        }
        else if (findMethod(argTwo) == 3)
        {
            value2 = atoi(&argTwo[2]);
        }
        *codeOne = (ABSOLUTE | (value << 6) | (value2 << 3));
    }
}

void listCoder(char* argOne, char* argTwo, int *codeOne, int *codeTwo)
{
    if (isLabel(argOne))
    {
        int labelValue = getLabelValue(argOne);
        *codeOne = (RELOCATABLE | (labelValue << 6));
    }
    if (isLabel(argTwo))
    {
        int labelValue = getLabelValue(argTwo);
        *codeTwo = (RELOCATABLE | (labelValue << 3));
    }

}

int opcode_coder(char* opcode, char* args)
{
    int code = 0;
    int opcodeNum = keyfromstring(opcode);
    //opcode
    code |= opcodeNum << OPCODE_OFFSET;
    char* argOne = NULL;
    char* argTwo = NULL;
    if (strlen(args) > 0)
    {
        split_args(args, &argOne, &argTwo);
        argOne = trimWhiteSpace(argOne);
        argTwo = trimWhiteSpace(argTwo);
        // Origin operand
        if (argOne == NULL)
        {
            code |= ABSOLUTE << ARE_OFFSET;
            return code;
        }
        else if (argTwo == NULL)
        {
            if (is_number_with_hash(argOne))
            {
                code |= IMMEDIATE << DESTINATION_METHOD_OFFSET;
            }

            else if (is_dereferenced_operand(argOne))
            {
                code |= REGISTER_INDIRECT << DESTINATION_METHOD_OFFSET;
            }
            else if (is_operand(argOne))
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
        else
        {
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
        }
    }
    code |= ABSOLUTE << ARE_OFFSET;
    return code;
}

bool movValidator(char* op, char* argOne, char* argTwo)
{
    if ((findMethod(argOne) == 1 || findMethod(argOne) == 2 || findMethod(argOne) == 4 || findMethod(argOne) == 3) && (findMethod(argTwo) == 1 || findMethod(argTwo) == 2 || findMethod(argTwo) == 4))
    {
        return true;
    }
    return false;
}

bool cmpValidator(char* op, char* argOne, char* argTwo)
{
    if ((findMethod(argOne) == 1 || findMethod(argOne) == 2 || findMethod(argOne) == 4 || findMethod(argOne) == 3) && (findMethod(argTwo) == 1 || findMethod(argTwo) == 2 || findMethod(argTwo) == 4 || findMethod(argTwo) == 3))
    {
        return true;
    }
    return false;
}

bool addValidator(char* op, char* argOne, char* argTwo)
{
    if ((findMethod(argOne) == 1 || findMethod(argOne) == 2 || findMethod(argOne) == 4 || findMethod(argOne) == 3) && (findMethod(argTwo) == 1 || findMethod(argTwo) == 2 || findMethod(argTwo) == 4))
    {
        return true;
    }
    return false;
}

bool subValidator(char* op, char* argOne, char* argTwo)
{
    if ((findMethod(argOne) == 1 || findMethod(argOne) == 2 || findMethod(argOne) == 4 || findMethod(argOne) == 3) && (findMethod(argTwo) == 1 || findMethod(argTwo) == 2 || findMethod(argTwo) == 4))
    {
        return true;
    }
    return false;
}

bool leaValidator(char* op, char* argOne, char* argTwo)
{
    if (findMethod(argOne) == 1 && (findMethod(argTwo) == 1 || findMethod(argTwo) == 2 || findMethod(argTwo) == 4))
    {
        return true;
    }
    return false;
}

bool clrValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == 1 || findMethod(argOne) == 2 || findMethod(argOne) == 4))
    {
        return true;
    }
    return false;
}

bool notValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == 1 || findMethod(argOne) == 2 || findMethod(argOne) == 4))
    {
        return true;
    }
    return false;
}

bool incValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == 1 || findMethod(argOne) == 2 || findMethod(argOne) == 4))
    {
        return true;
    }
    return false;
}

bool decValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == 1 || findMethod(argOne) == 2 || findMethod(argOne) == 4))
    {
        return true;
    }
    return false;
}

bool jmpValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == 1 || findMethod(argOne) == 2))
    {
        return true;
    }
    return false;
}

bool bneValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == 1 || findMethod(argOne) == 2))
    {
        return true;
    }
    return false;
}

bool redValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == 1 || findMethod(argOne) == 2 || findMethod(argOne) == 4))
    {
        return true;
    }
    return false;
}

bool prnValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == 1 || findMethod(argOne) == 2 || findMethod(argOne) == 4 || findMethod(argOne) == 3))
    {
        return true;
    }
    return false;
}

bool jsrValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == 1 || findMethod(argOne) == 2))
    {
        return true;
    }
    return false;
}

bool rtsValidator(char* op, char* argOne, char* argTwo)
{
    if (argOne == NULL && argTwo == NULL)
    {
        return true;
    }
    return false;
}

bool stopValidator(char* op, char* argOne, char* argTwo)
{
    if (argOne == NULL && argTwo == NULL)
    {
        return true;
    }
    return false;
}

bool validator(char* op, char* argOne, char* argTwo)
{
    switch (keyfromstring(op))
    {
        case OP_MOV:
            return movValidator(op, argOne, argTwo);
        case OP_CMP:
            return cmpValidator(op, argOne, argTwo);
        case OP_ADD:
            return addValidator(op, argOne, argTwo);
        case OP_SUB:
            return subValidator(op, argOne, argTwo);
        case OP_LEA:
            return leaValidator(op, argOne, argTwo);
        case OP_CLR:
            return clrValidator(op, argOne, argTwo);
        case OP_NOT:
            return notValidator(op, argOne, argTwo);
        case OP_INC:
            return incValidator(op, argOne, argTwo);
        case OP_DEC:
            return decValidator(op, argOne, argTwo);
        case OP_JMP:
            return jmpValidator(op, argOne, argTwo);
        case OP_BNE:
            return bneValidator(op, argOne, argTwo);
        case OP_RED:
            return redValidator(op, argOne, argTwo);
        case OP_PRN:
            return prnValidator(op, argOne, argTwo);
        case OP_JSR:
            return jsrValidator(op, argOne, argTwo);
        case OP_RTS:
            return rtsValidator(op, argOne, argTwo);
        case OP_STOP:
            return stopValidator(op, argOne, argTwo);
        case OP_UNKNOWN:
            return false;
    }
    return false;
}