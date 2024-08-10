typedef struct
{
    char* name;
    unsigned short value;
    char* type;
    int location;
} Label;

bool is_symbol_exist(char *symbol);
void add_symbol(char* name, unsigned short value, char* type);
int get_symbol_count();
void set_ic_offset(unsigned short offset);
void set_type(char *symbol, char* type);
Label** get_symbol_table();
void print_symbol_table();
Label* get_symbol(char *symbol);