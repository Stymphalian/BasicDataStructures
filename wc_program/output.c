#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "structures.h"
#include "misc.h"
#include "output.h"

void print_usage(int argc, char** argv){
	printf("Usage: %s [options]* [file]*\n", argv[0]);
	printf("Options:\n");
	printf("   -h,--help      -- \t Print out this usage message.\n");
	printf("   -v,--version   -- \t Print the version information for this utility.\n");
	printf("   -c,--chars     -- \t Number of characters.\n");
	printf("   -C,--max-chars -- \t Length of the longest line.\n");	
	printf("   -l,--lines     -- \t Number of lines.\n");
	printf("   -w,--words     -- \t Number of words.\n");
	printf("   -W,--max-words -- \t Number of words on the longest line.\n");			
	printf("   --padding=val  -- \t Set horizontal-padding between each number. Default=5\n"); 
	printf("   --print-header -- \t Print the format header.\n");
	printf("   --print-summary-- \t Print a summary of all the files parsed.\n");
	printf("   --exclude [file]+         -- \t A list of files to exclude form the parsing.\n");
	printf("   --source-files [file]+    -- \t Source files list files to parse.Line separated.\n");
	printf("   --word-occurences [word]+ -- \t Count the number occurences of [word].\n");
}
void print_version_info(struct stateinfo_t* si) {
	printf("%s : Word Count Utility\n", si->argv[0]);
	printf("Version :  %.2f\n",VERSION_ID);
	printf("Author : %s\n", "Jordan Yu");
	printf("Compile Date : %s %s\n", __DATE__, __TIME__);
}

void print_header(struct stateinfo_t* state) {
	int i = 0;
	for(i = 1; i < 5 +1 ; ++i ) {
		if( state->lines_flag == i) {printf("%s,","lines");continue;}
		if( state->words_flag==i) {printf("%s,", "words");continue;}
		if( state->chars_flag==i) {printf("%s,","char");continue;}
		if( state->max_words_line_flag==i) {printf("%s,","max-words");continue;}
		if( state->max_chars_line_flag==i) {printf("%s,","max-line");continue;}			
		if( state->word_occurences_flag==i) {
			int j = 0;
			for( j =0; j < state->word_occurences_size; ++j){
				printf("%s,",state->word_occurences[j]);
			}
			continue;
		}
	}
	printf("filename\n");
}

void print_information(struct wordcount_t* wct,struct stateinfo_t* state, char* filename) {
	int values[32 + state->word_occurences_size];
	memset(values,0,(32+state->word_occurences_size)*sizeof(int));

	int count = 0;
	int target = 0;
	for( target = 1; target< 5+1; ++target ) {
		if( state->lines_flag == target) {values[count++]= wct->lc;continue;}
		if( state->words_flag == target) {values[count++]= wct->wc;continue;}
		if( state->chars_flag == target) {values[count++]= wct->cc;continue;}
		if( state->max_words_line_flag == target) {values[count++]= wct->max_words_line;continue;}
		if( state->max_chars_line_flag == target) {values[count++]= wct->max_chars_line;continue;}
		if( state->word_occurences_flag == target) {
			int i = 0;
			for( i = 0; i < state->word_occurences_size; ++i) {
				values[count++] = wct->word_occurences_count[i];
			}
			continue;
		}
	}
	if( count == 0){ print_error(state,"No values - %s\n",filename);return;}

	// print each value. Ensuring it meets the padding criteria
	char* padded_num = get_padding_string(state->padding,'d');
	sprintf(padded_num,"%%%dd ", state->padding);
	int i = 0;
	for( i = 0; i < count ; ++i ){
		printf(padded_num,values[i]);
	}	

	// print the padded file names
	padded_num = get_padding_string(state->padding,'s');
	printf(padded_num, filename);
	printf("\n");
}


void print_summary(struct wordcount_t* wc, struct stateinfo_t* si ){
	char* padding = get_padding_string(si->padding,'s');
	printf(padding,"--\n");
	print_information(wc,si,"Totals");
}


void print_stateinfo(struct stateinfo_t* si){
	int i=0;

	printf("words_flag           = %d\n",si->words_flag);
	printf("chars_flag           = %d\n",si->chars_flag);
	printf("lines_flag           = %d\n",si->lines_flag);
	printf("max_words_line_flag  = %d\n",si->max_words_line_flag);
	printf("max_chars_line_flag  = %d\n",si->max_chars_line_flag);
	
	printf("padding              = %d\n",si->padding);
	printf("print_header_flag    = %d\n",si->print_header_flag);
	printf("print_summary_flag   = %d\n",si->print_summary_flag);

	printf("word_occurences_flag = %d\n",si->word_occurences_flag);
	printf("word_occurences_size = %d\n",si->word_occurences_size);
	printf("word_occurences      = ");
	for( i = 0; i < si->word_occurences_size;++i){
		printf("\"%s\" ",si->word_occurences[i]);
	}	
	printf("\n");


	printf("exclude_files_flag   = %d\n",si->exclude_files_flag);
	printf("exclude_files_size   = %d\n",si->exclude_files_size);
	printf("exclude_files        = ");
	for( i = 0 ; i < si->exclude_files_size ;++i){
		printf("\"%s\" ",si->exclude_files[i]);
	}
	printf("\n");

	printf("argc                 = %d\n",si->argc);
	printf("argv                 = ");
	for( i =0 ; i < si->argc; ++i ){
		printf("%s,", si->argv[i]);	
	}
	printf("\n");

	printf("consumed_args        = ");
	for( i = 0 ; i < si->argc; ++i){
		char* str = 0;
		switch(si->consumed_args[i]){
			case(OPT_none): str="none";break;
			case(OPT_normal): str="normal";break;
			case(OPT_source):str="source";break;								 
			case(OPT_consumed):str="consumed";break;
			default: str = "wtf";
		}
		printf("%s ",str);
	}
	printf("\n");
}


void print_error(struct stateinfo_t* si,const char* msg,...){
	if( si->quiet_flag == 0 ){
		va_list args;
		va_start(args, msg);
		fprintf(stderr,"ERROR: ");
		vfprintf(stderr,msg, args);
		va_end(args);
	}
}
#endif
