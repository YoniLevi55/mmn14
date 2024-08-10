
typedef struct {
    char* name;
    int size;
    unsigned short value;
    int IC;
} code_segment;

code_segment** get_code_segment();
int get_IC();
int get_code_segment_size();
void codeSegment_add_code(unsigned short value, char* name);
void printCodeSegment();
