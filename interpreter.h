typedef char* STRING;
typedef char bool;
#define TRUE 1
#define FALSE 0

// An entry in memory
typedef struct Variable
{
  char data;
  STRING name;
} Variable;


void scan_args(STRING s);
void read_input(FILE *source);
void process_line(STRING line);
void do_adjustment(unsigned long long line_size);
unsigned long long variable_address(STRING element);
bool are_equivalent(STRING s1, STRING s2);
void convert_to_upper(STRING src, STRING dest);
bool check_malloc(void* pointer);
