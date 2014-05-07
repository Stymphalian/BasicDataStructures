#include <stdio.h>

// constants, enums, and defines
#define VERSION_ID 2.40
#define DEFAULT_PADDING 5
enum option_type {OPT_none,OPT_consumed, OPT_source,OPT_normal};

// functions
int iswhitespace(char c);
int getline(char** line, unsigned* linesize, FILE* file);
char* get_padding_string( int padding, char type);
int str_find_array(char* file, char** list, int size);
