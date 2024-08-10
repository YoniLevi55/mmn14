
typedef struct {
    char* name;
    int size;
    int value;
    int IC;
} code_segment;

code_segment** get_code_segment();
int get_IC();
int get_code_segment_size();
void codeSegment_add_code(int value, char* name);
void printCodeSegment();
