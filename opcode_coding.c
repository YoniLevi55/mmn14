#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "string_helper.h"
#include "opcode_coding.h"
#include "first_pass.h"
enum OPCODES //enum for the opcodes
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

typedef struct { char *key; int val; } t_symstruct; //struct for the opcodes

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

enum OPCODES keyfromstring(char *key) //get the opcode from the string
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


#define OPCODE_OFFSET 11 //opcode offset
#define ORIGIN_METHOD_OFFSET 7 //origin method offset
#define DESTINATION_METHOD_OFFSET 3 //destination method offset
#define ARE_OFFSET 0 //ARE offset

bool isOperation(char* line) //checks if the line is an operation
{
    char* trimmedLine = trimWhiteSpace(line);

    return (keyfromstring(trimmedLine)!= OP_UNKNOWN);
}

bool isDataType(char* line) //checks if the line is a data type
{
    char* trimmedLine = trimWhiteSpace(line);
    if (strcmp(trimmedLine, ".data") == 0 || strcmp(trimmedLine, ".string") == 0 || strcmp(trimmedLine, ".extern") == 0 || strcmp(trimmedLine, ".entry") == 0)
    {
        return true;
    }
        return false;
}

bool is_number(const char* str) //checks if the string is a number
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

bool is_number_with_hash(const char* str) //checks if the string is a number with a hash
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

bool is_operand(const char* str) //checks if the string is an operand
{
   if ((str != NULL) && (strcmp(str, "r1") == 0 || strcmp(str, "r2") == 0 || strcmp(str, "r3") == 0 || strcmp(str, "r4") == 0 || strcmp(str, "r5") == 0 || strcmp(str, "r6") == 0 || strcmp(str, "r7") == 0 || strcmp(str, "r0") == 0))
   {
        return true;
   }
   return false;
}

bool is_dereferenced_operand(const char* str) //checks if the string is a dereferenced operand
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

void split_args(const char* args, char** argOne, char** argTwo) //splits the arguments
{
    if (args == NULL || strcmp(args, "") == 0) //if there are no arguments
    {
        *argOne = NULL;
        *argTwo = NULL;
        return;
    }
    int count = 0;
    char** splitted = split_string(args, ',', &count); //splits the arguments by the comma
    if (count == 1) //if there is one argument
    {
        *argOne = malloc(strlen(splitted[0]));
        strcpy(*argOne, trimWhiteSpace(splitted[0]));
    }
    else if (count == 2) //if there are two arguments
    {
        *argOne = malloc(strlen(splitted[0]));
        *argTwo = malloc(strlen(splitted[1]));
        strcpy(*argOne, trimWhiteSpace(splitted[0]));
        strcpy(*argTwo, trimWhiteSpace(splitted[1]));
    }
}

int getNumOfArgs(const char* args) //gets the number of arguments
{
    int count = 0;
    char** splitted = split_string(args, ',', &count);
    return count;
}



enum METHODS findMethod(char* arg) //finds the method of a given argument.
{
    if (is_number_with_hash(arg))
    {
        return IMMEDIATE;
    }
    else if (is_dereferenced_operand(arg))
    {
        return REGISTER_INDIRECT;
    }
    else if (is_operand(arg))
    {
        return REGISTER_DIRECT;
    }
    else
    {
        return DIRECT;
    }
}

unsigned short AtoUnsignedShrt(char* str) //converts a string to an unsigned short
{
    unsigned short value = 0;
    if (is_number_with_hash(str))
    {
        int readValue = atoi(&str[1]);
        value = (unsigned short)readValue;
    }
    else{
        int readValue = atoi(str);
        value = (unsigned short)readValue;
    }
    return value;
}

void operandCoder(char* argOne, char* argTwo, unsigned short *codeOne, unsigned short *codeTwo) //codes the operands
{
    argOne = trimWhiteSpace(argOne);
    argTwo = trimWhiteSpace(argTwo);
    if (findMethod(argOne) == IMMEDIATE)
    {
        unsigned short value = AtoUnsignedShrt(&argOne[1]);
        *codeOne = ( (unsigned short) ABSOLUTE | (value << 3));
    }
    if (findMethod(argTwo) == IMMEDIATE)
    {
        unsigned short value = AtoUnsignedShrt(&argTwo[1]);
        *codeTwo =  (  (unsigned short) ABSOLUTE | (value << 3));
    }
    if (findMethod(argOne) == DIRECT)
    {
        *codeOne = (unsigned short) -1;
    }
    if (findMethod(argTwo) == DIRECT)
    {
        *codeTwo = (unsigned short) -1;
    }
    if (findMethod(argOne) == REGISTER_DIRECT  && argTwo == NULL)
    {
        unsigned short value = AtoUnsignedShrt(&argOne[1]);
        *codeOne = ( (unsigned short) ABSOLUTE | (value << 3));
    }
    else if (findMethod(argOne) == REGISTER_DIRECT && (findMethod(argTwo) == IMMEDIATE || findMethod(argTwo) == DIRECT))
    {
        unsigned short value = AtoUnsignedShrt(&argOne[1]);
        *codeOne = ( (unsigned short)ABSOLUTE | (value << 6));
    }
    else if (findMethod(argTwo) == REGISTER_DIRECT && (findMethod(argOne) == IMMEDIATE|| findMethod(argOne) == DIRECT))
    {
        unsigned short value = AtoUnsignedShrt(&argTwo[1]);
        *codeTwo = ( (unsigned short)ABSOLUTE | (value << 3));
    }
    else if (findMethod(argOne) == REGISTER_INDIRECT && argTwo == NULL)
    {
        unsigned short value = AtoUnsignedShrt(&argOne[2]);
        *codeOne = ( (unsigned short)ABSOLUTE | (value << 3));
    }
    else if (findMethod(argOne) == REGISTER_INDIRECT && (findMethod(argTwo) == IMMEDIATE|| findMethod(argTwo) == DIRECT))
    {
        unsigned short value = AtoUnsignedShrt(&argOne[2]);
        *codeOne = ( (unsigned short)ABSOLUTE | (value << 6));
    }
    else if (findMethod(argTwo) == REGISTER_INDIRECT && (findMethod(argOne) == IMMEDIATE|| findMethod(argOne) == DIRECT ))
    {
        unsigned short value = AtoUnsignedShrt(&argTwo[2]);
        *codeTwo = ( (unsigned short)ABSOLUTE | (value << 3));
    }
    else if ((findMethod(argOne) == REGISTER_DIRECT || findMethod(argOne) == REGISTER_INDIRECT) && (findMethod(argTwo) == REGISTER_DIRECT || findMethod(argTwo) == REGISTER_INDIRECT))
    {
        unsigned short value = 0;
        unsigned short value2  = 0;
        if (findMethod(argOne) == REGISTER_DIRECT)
        {
            value = AtoUnsignedShrt(&argOne[1]);
        }
        else if (findMethod(argOne) == REGISTER_INDIRECT)
        {
            value = AtoUnsignedShrt(&argOne[2]);
        }
        if (findMethod(argTwo) == REGISTER_DIRECT)
        {
            value2 = AtoUnsignedShrt(&argTwo[1]);
        }
        else if (findMethod(argTwo) == REGISTER_INDIRECT)
        {
            value2 = AtoUnsignedShrt(&argTwo[2]);
        }
        *codeOne = ( (unsigned short)ABSOLUTE | (value << 6) | (value2 << 3));
    }
    if (argOne == NULL)
    {
        *codeOne = 0;
    }
    if (argTwo == NULL)
    {
        *codeTwo = 0;
    }
}

void labelCoder(char* arg, unsigned short *code, char* type) //codes the label
{
    if (findMethod(arg) == DIRECT)
    {
        if (strcmp(type, ".extern") == 0)
        {
            *code = (EXTERNAL | (0 << 3));
        }
        else
        {
            unsigned short labelValue = getLabelValue(arg);
            *code = (RELOCATABLE | (labelValue << 3));
        }
    }

}

unsigned short opcode_coder(char* opcode, char* args) //codes the opcode
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

//validator functions for the operations
bool movValidator(char* op, char* argOne, char* argTwo)
{
    if ((findMethod(argOne) == IMMEDIATE || findMethod(argOne) == DIRECT || findMethod(argOne) == REGISTER_DIRECT || findMethod(argOne) == REGISTER_INDIRECT) && (findMethod(argTwo) == IMMEDIATE || findMethod(argTwo) == DIRECT || findMethod(argTwo) == REGISTER_DIRECT))
    {
        return true;
    }
    return false;
}

bool cmpValidator(char* op, char* argOne, char* argTwo)
{
    if ((findMethod(argOne) == IMMEDIATE || findMethod(argOne) == DIRECT || findMethod(argOne) == REGISTER_DIRECT || findMethod(argOne) == REGISTER_INDIRECT) && (findMethod(argTwo) == IMMEDIATE || findMethod(argTwo) == DIRECT || findMethod(argTwo) == REGISTER_DIRECT || findMethod(argTwo) == REGISTER_INDIRECT))
    {
        return true;
    }
    return false;
}

bool addValidator(char* op, char* argOne, char* argTwo)
{
    if ((findMethod(argOne) == IMMEDIATE || findMethod(argOne) == DIRECT || findMethod(argOne) == REGISTER_DIRECT || findMethod(argOne) == REGISTER_INDIRECT) && (findMethod(argTwo) == DIRECT || findMethod(argTwo) == REGISTER_INDIRECT || findMethod(argTwo) == REGISTER_DIRECT))
    {
        return true;
    }
    return false;
}

bool subValidator(char* op, char* argOne, char* argTwo)
{
    if ((findMethod(argOne) == IMMEDIATE || findMethod(argOne) == DIRECT || findMethod(argOne) == REGISTER_DIRECT || findMethod(argOne) == REGISTER_INDIRECT) && (findMethod(argTwo) == IMMEDIATE || findMethod(argTwo) == DIRECT || findMethod(argTwo) == REGISTER_DIRECT))
    {
        return true;
    }
    return false;
}

bool leaValidator(char* op, char* argOne, char* argTwo)
{
    if (findMethod(argOne) == DIRECT && (findMethod(argTwo) == DIRECT || findMethod(argTwo) == REGISTER_INDIRECT || findMethod(argTwo) == REGISTER_DIRECT))
    {
        return true;
    }
    return false;
}

bool clrValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == IMMEDIATE || findMethod(argOne) == DIRECT || findMethod(argOne) == REGISTER_DIRECT))
    {
        return true;
    }
    return false;
}

bool notValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == IMMEDIATE || findMethod(argOne) == DIRECT || findMethod(argOne) == REGISTER_DIRECT))
    {
        return true;
    }
    return false;
}

bool incValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == IMMEDIATE || findMethod(argOne) == DIRECT || findMethod(argOne) == REGISTER_DIRECT))
    {
        return true;
    }
    return false;
}

bool decValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == IMMEDIATE || findMethod(argOne) == DIRECT || findMethod(argOne) == REGISTER_DIRECT))
    {
        return true;
    }
    return false;
}

bool jmpValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == IMMEDIATE || findMethod(argOne) == DIRECT))
    {
        return true;
    }
    return false;
}

bool bneValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == IMMEDIATE || findMethod(argOne) == DIRECT))
    {
        return true;
    }
    return false;
}

bool redValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == IMMEDIATE || findMethod(argOne) == DIRECT || findMethod(argOne) == REGISTER_DIRECT))
    {
        return true;
    }
    return false;
}

bool prnValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == IMMEDIATE || findMethod(argOne) == DIRECT || findMethod(argOne) == REGISTER_DIRECT || findMethod(argOne) == REGISTER_INDIRECT))
    {
        return true;
    }
    return false;
}

bool jsrValidator(char* op, char* argOne, char* argTwo)
{
    if (argTwo == NULL && (findMethod(argOne) == IMMEDIATE || findMethod(argOne) == DIRECT))
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

bool validator(char* op, char* argOne, char* argTwo) //validator function for the operations
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