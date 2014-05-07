#ifndef OUTPUT_H
#define OUTPUT_H

#include "structures.h"

int print_error(const char* message,...);
int print_usage(int argc,char** argv);
int print_version(stateinfo_t* state);
int print_stateinfo(stateinfo_t* state);
int print_header(stateinfo_t* state, entry_t* entry);

#endif
