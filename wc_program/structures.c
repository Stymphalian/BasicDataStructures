#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "output.h"
#include "structures.h"

void init_stateinfo(struct stateinfo_t* si,int argc, char** argv){
	if(!si || !argv) {return;}
	si->words_flag = 0;
	si->chars_flag = 0;
	si->lines_flag = 0;
	si->max_words_line_flag = 0;
	si->max_chars_line_flag = 0;
	si->print_header_flag = 0;
	si->print_summary_flag = 0;

	si->word_occurences_flag = 0;
	si->word_occurences = malloc(sizeof(char*)*argc);
	si->word_occurences_size = 0;
	if( !si->word_occurences) {
		fprintf(stderr,"Error initializing stateinfo word_occurences array.\n");
	}else{
		memset(si->word_occurences,0,sizeof(char*)*argc);
	}


	si->exclude_files_flag = 0;
	si->exclude_files = malloc(sizeof(char*)*argc);
	si->exclude_files_size = 0;
	if(!si->exclude_files ){
		fprintf(stderr,"Error initlializing stateinfo exlude files array.\n");
	}else{
		memset(si->exclude_files,0, sizeof(char*)*argc);
	}
	
	si->padding = 0;
	si->quiet_flag =0;
	si->file_flag = 0;
	si->argc = argc;
	si->argv = argv;	

	si->consumed_args = malloc(sizeof(int)*argc);
	if(!si->consumed_args  ) {
		fprintf(stderr,"Error initializing stateinfo concumed_args array.\n");
	}else{
		memset(si->consumed_args, OPT_none, sizeof(int)*argc);
	}
}
void destroy_stateinfo(struct stateinfo_t* si ){
	if(!si){return;}
	free(si->consumed_args);
	free(si->word_occurences);
	free(si->exclude_files);
	memset(si,0, sizeof(struct stateinfo_t));
}


void init_wordcount(struct wordcount_t* wc, struct stateinfo_t* state){
	wc->wc = 0;
	wc->cc = 0;
	wc->lc = 0;
	wc->max_words_line = 0;
	wc->max_chars_line =0;

	wc->word_occurences_count = malloc(sizeof(int)*(state->word_occurences_size+1));
	if(!wc->word_occurences_count){
		print_error(state,"Error initializing wordcount.word_occurenes_count\n");
	}else{
		memset(wc->word_occurences_count,0,sizeof(int)*(state->word_occurences_size+1));
	}
}
void clear_wordcount(struct wordcount_t* wc){
	if(!wc){return ;}
	free(wc->word_occurences_count);
	memset(wc,0,sizeof(struct wordcount_t));
}

int add_to_summary(struct wordcount_t* summary, struct wordcount_t* wc, struct stateinfo_t* state){
	if( !summary || !wc || !state) {return -1;}
	summary->wc += wc->wc;
	summary->cc += wc->cc;
	summary->lc += wc->lc;
	summary->max_words_line += wc->max_words_line;
	summary->max_chars_line += wc->max_chars_line;

	if( summary->word_occurences_count == 0 || wc->word_occurences_count == 0 ){
		return -1;
	}

	int i = 0;
	for(i =0 ; i < state->word_occurences_size; ++i){
		summary->word_occurences_count[i] += wc->word_occurences_count[i];
	}	
	return 0;
}

#endif
