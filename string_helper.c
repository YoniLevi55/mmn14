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
        perror("Unable to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Allocate memory and copy each token
    int start = 0;
    int token_index = 0;
    for (int i = 0; i <= length; i++) {
        if (str[i] == delimiter || str[i] == '\0') {
            int token_length = i - start;
            tokens[token_index] = (char*)malloc((token_length + 1) * sizeof(char));
            if (tokens[token_index] == NULL) {
                perror("Unable to allocate memory");
                exit(EXIT_FAILURE);
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
