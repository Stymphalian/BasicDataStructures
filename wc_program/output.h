#include "structures.h"
void print_error(struct stateinfo_t* si, const char* msg,...);
void print_usage(int argc, char** argv);
void print_header(struct stateinfo_t* state);
void print_version_info(struct stateinfo_t* state);
void print_information(struct wordcount_t*, struct stateinfo_t*, char* filename);
void print_summary( struct wordcount_t*, struct stateinfo_t*);
void print_stateinfo(struct stateinfo_t*  state);
