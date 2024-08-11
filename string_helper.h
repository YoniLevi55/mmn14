#include <stdbool.h>
char* trimWhiteSpace(char* line);
int findFirstSign(char *line, char sign);
char** split_string(const char* str, const char delimiter, int* count);
bool isLabel(char *line);
char* getLabel(char* line);
void removeLastChar(char* str);
char *removeQuotes(const char *str);
void breakLine(char* line, char** label, char** operation, char** datatype, char** args);
char *strdup(const char *str);