#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include "errors_handler.h"
char* trimWhiteSpace(char* str)
{
    if (str == NULL)
    {
        return NULL;
    }
    // Skip leading whitespace
    while (isspace(*str)) {
        str++;
    }

    // Skip trailing whitespace
    char* end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end--;
    }

    // Allocate memory for the trimmed string
    char* result = malloc(end - str + 2);
    if (result == NULL) {
        exit_with_error(EXIT_FAILURE, "Failed to allocate memory");
    }

    // Copy the trimmed string
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

    // First pass to count the number of tokens
    for (int i = 0; i < length; i++) {
        if (str[i] == delimiter) {
            token_count++;
        }
    }
    token_count++;  // Add one for the last token

    // Allocate memory for the array of strings
    char** tokens = (char**)malloc(token_count * sizeof(char*));
    if (tokens == NULL) {
        exit_with_error(EXIT_FAILURE, "Failed to allocate memory");
    }

    // Allocate memory and copy each token
    int start = 0;
    int token_index = 0;
    for (int i = 0; i <= length; i++) {
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

    // Update the count
    *count = token_count;

    return tokens;
}

bool isLabel(char *line)
{
    char* trimmedLine = trimWhiteSpace(line);
    int colonPos = findFirstSign(trimmedLine, ':');
    bool validLabel = colonPos >= 1 && ((trimmedLine[0] > 64 && trimmedLine[0] < 91) || (trimmedLine[0] > 96 && trimmedLine[0] < 123));
    if (validLabel) //checks if their is a valid label, checks that first character is a capital letter and that the colon is not the first character.
    {
        return true;
    }
    else //there is no valid label.
    {
        return false;
    }
}

char* getLabel(char* line)
{
    int colonPos = findFirstSign(line,':');
    char* label = malloc(colonPos - 1);
    strncpy(label, line, colonPos - 1);
    return label;
}

void removeLastChar(char* str) {
    int length = strlen(str);

    if (length > 0) {
        str[length - 1] = '\0';  // Set the last character to null terminator
    }
}

char *removeQuotes(const char *str) {
    int length = strlen(str);
    char *result = (char *)malloc(length + 1); // Allocate memory for the new string
    if (result == NULL) {
        printf("Memory allocation failed\n");
        exit(1); // Exit if memory allocation fails
    }

    int j = 0;

    // Traverse the string and copy characters that are not quotes
    for (int i = 0; i < length; i++) {
        if (str[i] != '"') {
            result[j++] = str[i];
        }
    }

    // Null-terminate the new string
    result[j] = '\0';

    return result;
}