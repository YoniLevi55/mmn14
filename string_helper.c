#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
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
    // Initialize count to zero
    *count = 0;

    // Copy the input string to avoid modifying the original
    char* str_copy = strdup(str);
    if (str_copy == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Count the number of tokens
    char* temp = str_copy;
    while (*temp) {
        if (*temp == delimiter) {
            (*count)++;
        }
        temp++;
    }
    (*count)++;  // Number of tokens is number of delimiters + 1

    // Allocate memory for the array of strings
    char** result = malloc((*count) * sizeof(char*));
    if (result == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    if (*count == 1) {
        result[0] = strdup(str_copy);
        if (result[0] == NULL) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        free(str_copy);
        return result;
    }

    // Split the string into tokens
    int index = 0;
    char* token = strtok(str_copy, &delimiter);
    while (token != NULL) {
        result[index] = strdup(token);
        if (result[index] == NULL) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        index++;
        token = strtok(NULL, &delimiter);
    }

    // Free the copy of the input string
    free(str_copy);

    return result;
}

