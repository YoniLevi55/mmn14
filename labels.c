#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int findFirstPosition(char *str, char ch) {
    char *pos = strchr(str, ch);
    if (pos) {
        return pos - str;
    } else {
        return -1; // Character not found
    }
}


void copySubstring(char *source, int start, int length, char *destination) {
    if (start < 0 || start >= strlen(source)) {
        printf("Invalid start position.\n");
        destination[0] = '\0'; // Return an empty string
        return;
    }
    if (length < 0 || start + length > strlen(source)) {
        printf("Invalid length.\n");
        destination[0] = '\0'; // Return an empty string
        return;
    }
    strncpy(destination, source + start, length);
    destination[length] = '\0'; // Null-terminate the destination string

}

int isLabel(char *line, char *label) {
    int colon = findFirstPosition(line, ':');

    if (colon!=-1) {
        copySubstring(line, 0, colon, label);
        printf("Found label: %s\n", label);
        return 1;
    }
    printf("No label found\n");
    return 0;
}
