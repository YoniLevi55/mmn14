#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "errors_handler.h"
#include "opcode_coding.h"
char* trimWhiteSpace(char* str)
{
    char* end;
    char* result;

    if (str == NULL)
    {
        return NULL;
    }
    /* Skip leading whitespace */
    while (isspace(*str)) {
        str++;
    }

    /* Skip trailing whitespace */
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end--;
    }

    /* Allocate memory for the trimmed string */
    result = malloc(end - str + 2);
    if (result == NULL) {
        exit_with_error(EXIT_FAILURE, "Failed to allocate memory");
    }

    /* Copy the trimmed string */
    strncpy(result, str, end - str + 1);
    result[end - str + 1] = '\0';

    return result;
}

int findFirstSign(char *line, char sign)
{
    char *pos = strchr(line, sign);
    if (pos)
    {
        return pos - line;
    }
    else
    {
        return -1;
    }
}

char** split_string(const char* str, const char delimiter, int* count) {
    int length = strlen(str);
    int token_count = 0;
    char** tokens;
    int start = 0;
    int token_index = 0;
    int i=0;

    /* First pass to count the number of tokens */
    for (i = 0; i < length; i++) {
        if (str[i] == delimiter) {
            token_count++;
        }
    }
    token_count++;  /* Add one for the last token */

    /* Allocate memory for the array of strings */
    tokens = (char**)malloc(token_count * sizeof(char*));
    if (tokens == NULL) {
        exit_with_error(EXIT_FAILURE, "Failed to allocate memory");
    }
    if (tokens == NULL) {
        exit_with_error(EXIT_FAILURE, "Failed to allocate memory");
    }

    /* Allocate memory and copy each token */

    for (i = 0; i <= length; i++) {
        if (str[i] == delimiter || str[i] == '\0') {
            int token_length = i - start;
            tokens[token_index] = (char*)malloc((token_length + 1) * sizeof(char));
            if (tokens[token_index] == NULL) {
                exit_with_error(EXIT_FAILURE, "Failed to allocate memory");
            }
            strncpy(tokens[token_index], str + start, token_length);
            tokens[token_index][token_length] = '\0';
            token_index++;
            start = i + 1;
        }
    }

    *count = token_count;

    return tokens;
}

bool isLabel(char *line) /*checks if the line is a label.*/
{
    char* trimmedLine = trimWhiteSpace(line);
    int colonPos = findFirstSign(trimmedLine, ':');
    bool validLabel = colonPos >= 1 && ((trimmedLine[0] > 64 && trimmedLine[0] < 91) || (trimmedLine[0] > 96 && trimmedLine[0] < 123));
    if (validLabel) /*checks if their is a valid label, checks that first character is a capital letter and that the colon is not the first character.*/
    {
        return true;
    }
    else /*there is no valid label.*/
    {
        return false;
    }
}

char* getLabel(char* line) /*gets the label from the line.*/
{
    int colonPos = findFirstSign(line,':');
    char* label = malloc(colonPos - 1);
    if (label == NULL)
    {
        exit_with_error(EXIT_FAILURE, "Failed to allocate memory");
    }
    strncpy(label, line, colonPos - 1);
    return label;
}

void removeLastChar(char* str) /*removes the last character from the string.*/
{
    int length = strlen(str);
    if (length > 0)
    {
        str[length - 1] = '\0'; /*sets the last character to null terminator*/
    }
}

char *removeQuotes(const char *str)
{
    int j=0;
    int i=0;
    int length = strlen(str);
    char *result = (char *)malloc(length + 1); /* Allocate memory for the new string*/
    if (result == NULL) {
        printf("Memory allocation failed\n");
        exit(1); /* Exit if memory allocation fails*/
    }


    /* Traverse the string and copy characters that are not quotes*/
    for (i = 0; i < length; i++) {
        if (str[i] != '"') {
            result[j++] = str[i];
        }
    }

    /* Null-terminate the new string*/
    result[j] = '\0';

    return result;
}

void breakLine(char* line, char** label, char** operation, char** datatype, char** args) /*breaks the line into label, operation, datatype and args.*/
{
    int count;
    char** splitted = split_string(line, ' ', &count);
    int i=0;
    int offset = 0;
    if (isLabel(splitted[0])){
        *label = malloc(32);/* change allocation size in future.*/
        if (*label == NULL)
        {
            exit_with_error(EXIT_FAILURE, "Failed to allocate memory");
        }
        strcpy(*label, splitted[0]);
        offset+=strlen(splitted[0]);
        i++;
    }
    if (isDataType(splitted[i]) || isOperation(splitted[i]))
    {
        if (isDataType(splitted[i]))
        {
            *datatype = malloc(32); /* change allocation size in future.*/
            if (*datatype == NULL)
            {
                exit_with_error(EXIT_FAILURE, "Failed to allocate memory");
            }
            strcpy(*datatype, splitted[i]);
            offset+=strlen(splitted[i]);
        }
        else
        {
            *operation = malloc(32); /* change allocation size in future.*/
            if (*operation == NULL)
            {
                exit_with_error(EXIT_FAILURE, "Failed to allocate memory");
            }
            strcpy(*operation, splitted[i]);
            offset+=strlen(splitted[i]);
        }
        i++;
    }
    *args = malloc(strlen(line) - offset);
    if (*args == NULL)
    {
        exit_with_error(EXIT_FAILURE, "Failed to allocate memory");
    }
    strncpy(*args, &line[offset+1], strlen(line) - offset); /*make sure that if data is string, return without quotes.*/
}
char *strdup(const char *str)
{
    int n = strlen(str) + 1;
    char *dup = malloc(n);
    if(dup)
    {
        strcpy(dup, str);
    }
    return dup;
}
