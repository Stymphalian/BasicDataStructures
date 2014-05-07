#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "structures.h"
#include "misc.h"

void init_stateinfo_t(stateinfo_t* si,int argc, char** argv){
	memset(si,0,sizeof(stateinfo_t));
	si->argc = argc;
	si->argv = argv;
	si->consumed_args = malloc(sizeof(char*)*argc);

	memset(si->consumed_args,OPT_none,sizeof(char*)*argc);
}
void clear_stateinfo_t(stateinfo_t* si){
	free(si->consumed_args);
	memset(si,0,sizeof(stateinfo_t));
}
