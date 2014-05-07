#ifndef PARSER_H
#define PARSER_H

#include "structures.h"

int parse_args(stateinfo_t* state, int argc,char** argv);
int parse_config(stateinfo_t* state, const char* filename);

#endif
