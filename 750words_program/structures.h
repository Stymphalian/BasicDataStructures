#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include "misc.h"

typedef struct stateinfo_t{
	int argc;
	char** argv;

	int rewrite_flag;
	int no_header_flag;
	int author_flag;
	int editor_flag;
	int wc_flag;
	int config_flag;
	int count_flag;
	int dest_flag;

	char editor[MAX_PATH];
	char wc[MAX_PATH];
	char dest[MAX_PATH];
	char author[MAX_FILENAME];
	char config[MAX_FILENAME];

	int* consumed_args;
} stateinfo_t;


typedef struct entry_t{
	char path[MAX_PATH];
	char entry[MAX_FILENAME];
	char author[MAX_FILENAME];
	FILE* file;
} entry_t;

void init_stateinfo_t(stateinfo_t* state, int argc, char** argv);
void clear_stateinfo_t(stateinfo_t* state);
#endif
