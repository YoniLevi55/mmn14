typedef struct
{
    char* name;
    int value;
    char* type;
} Label;

bool is_symbol_exist(char *symbol);
void add_symbol(char* name, int value, char* type);
int get_symbol_count();
void set_ic_offset(int offset);
void set_type(char *symbol, char* type);
Label** get_symbol_table();
void print_symbol_table();