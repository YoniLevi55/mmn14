#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "logger.h"
#include "symbol_table.h"

Label** symbol_table;
int symbol_count = 0;

void add_symbol(char* name, unsigned short value, char* type){
    Label* newLabel = (Label*)malloc(sizeof(Label));
    if(newLabel == NULL){
        logger(ERROR,"Failed to allocate memory for symbol\n");
        exit(1);
    }
    logger(DEBUG,"Adding symbol: %s, value: %d, type: %s\n",name, value, type);
    newLabel->name = malloc(strlen(name)+1);
    newLabel->type = malloc(strlen(type)+1);
    if(newLabel->name == NULL || newLabel->type == NULL){
        logger(ERROR,"Failed to allocate memory for symbol name or type\n");
        exit(1);
    }
    newLabel->value = value;
    strcpy(newLabel->name, name);
    strcpy(newLabel->type, type);
    symbol_table = (Label**)realloc(symbol_table, sizeof(Label*) * (symbol_count+1));
    symbol_table[symbol_count] = newLabel;
    symbol_count++;
}

bool is_symbol_exist(char *symbol){
{
    int i=0;
    if (symbol == NULL)
    {
        return false;
    }
    if (symbol_table == NULL)
    {
        return false;
    }
    for (i = 0; i < symbol_count; i++)
    {
        if (strcmp(symbol_table[i]->name, symbol) == 0)
        {
            return true;
        }
    }
    return false;
}
}

int get_symbol_count(){
    return symbol_count;
}

Label* get_symbol(char *symbol){
    int i=0;
    for (i = 0; i < symbol_count; i++)
    {
        if (strcmp(symbol_table[i]->name, symbol) == 0)
        {
            return symbol_table[i];
        }
    }
    return NULL;
}


unsigned short getLabelValue(char* label)
{
    int i=0;
    for (i = 0; i < sizeof(symbol_table); i++)
    {
        if (strcmp(symbol_table[i]->name, label) == 0)
        {
            return symbol_table[i]->value;
        }
    }
    return -1;
}

void set_ic_offset(unsigned short offset){
    int i=0;
    for (i = 0; i < symbol_count; i++)
    {
        if (strcmp(symbol_table[i]->type, ".data") == 0)
            symbol_table[i]->value += offset + 100;
    }
}

void set_type(char *symbol, char* type){
    int i=0;
    for (i = 0; i < symbol_count; i++)
    {
        if (strcmp(symbol_table[i]->name, symbol) == 0)
        {
            symbol_table[i]->type = realloc(symbol_table[i]->type, strlen(type)+1);
            strcpy(symbol_table[i]->type, type);
        }
    }
}

void print_symbol_table(){
    int i=0;
    logger(DEBUG,"Symbol Table:\n");
    logger(DEBUG,"size: %d\n", symbol_count);
    for ( i = 0; i < symbol_count; i++)
    {
        logger(DEBUG,"Name: %s, Value: %d, Type: %s\n", symbol_table[i]->name, symbol_table[i]->value, symbol_table[i]->type);
    }
}

Label** get_symbol_table(){
    return symbol_table;
}
