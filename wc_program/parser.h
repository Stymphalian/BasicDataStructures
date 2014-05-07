#include "structures.h"
int parsefile(struct wordcount_t* wc, struct stateinfo_t* state, FILE* fin);
int parse_sourcefile(struct wordcount_t* wc, struct stateinfo_t* state, FILE* fin);
int parseargs(struct stateinfo_t* stateinfo , int argc, char** argv);

