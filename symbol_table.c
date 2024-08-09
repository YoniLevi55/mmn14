#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "symbol_table.h"

Label** symbol_table;
int symbol_count = 0;

void add_symbol(char* name, int value, char* type){
    Label* newLabel = (Label*)malloc(sizeof(Label));
    newLabel->name = malloc(strlen(name)+1);
    newLabel->type = malloc(strlen(type)+1);
    newLabel->value = value;
    strcpy(newLabel->name, name);
    strcpy(newLabel->type, type);
    symbol_table = (Label**)realloc(symbol_table, sizeof(Label*) * (symbol_count+1));
    symbol_table[symbol_count] = newLabel;
    symbol_count++;
}

bool is_symbol_exist(char *symbol){
{
    if (symbol == NULL)
    {
        return false;
    }
    if (symbol_table == NULL)
    {
        return false;
    }
    for (int i = 0; i < sizeof(symbol_table)/sizeof(Label); i++)
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
    for (int i = 0; i < sizeof(symbol_table)/sizeof(Label); i++)
    {
        if (strcmp(symbol_table[i]->name, symbol) == 0)
        {
            return symbol_table[i];
        }
    }
    return NULL;
}


int getLabelValue(char* label)
{
    for (int i = 0; i < sizeof(symbol_table); i++)
    {
        if (strcmp(symbol_table[i]->name, label) == 0)
        {
            return symbol_table[i]->value;
        }
    }
    return -1;
}

void set_ic_offset(int offset){
    for (int i = 0; i < sizeof(symbol_table)/sizeof(Label); i++)
    {
        if (strcmp(symbol_table[i]->type, ".data") == 0)
            symbol_table[i]->value += offset + 100;
    }
}

void set_type(char *symbol, char* type){
    for (int i = 0; i < sizeof(symbol_table)/sizeof(Label); i++)
    {
        if (strcmp(symbol_table[i]->name, symbol) == 0)
        {
            symbol_table[i]->type = realloc(symbol_table[i]->type, strlen(type)+1);
            strcpy(symbol_table[i]->type, type);
        }
    }
}

Label** get_symbol_table(){
    return symbol_table;
}