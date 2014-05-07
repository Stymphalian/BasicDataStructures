#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structures.h"
#include "misc.h"
#include "output.h"
#include "parser.h"

int parseargs(struct stateinfo_t* stateinfo, int argc, char** argv){
	int i = 0;
	int opt_flag = 0; // to note that an option has been hit.
	int file_flag = 0;
	int order_num = 1;

	int source_flag = 0; // --source-files
	int word_occurences_flag = 0; // --word-occurences
	int exclude_files_flag = 0; // --exclude

	#define strcmp_arg(arg) (strcmp(argv[i], arg) == 0)
	for( i = 1;i < argc; ++i ){

		/* START this is how we handle options which need to take in parameters.
		   A set of 'if' which look for a flag that is set when the option is found. */
		if(source_flag ) {
			if( argv[i][0] == '-' ){
			  	source_flag = 0;
			} else {
				file_flag = 1;
				stateinfo->consumed_args[i] = OPT_source;
				continue; // We want this continue here in order to avoid the logic below.
			}
		} else if( word_occurences_flag ){
			if( argv[i][0] == '-' ) {
			  	word_occurences_flag = 0;
			} else{
				stateinfo->consumed_args[i] = OPT_consumed;
				stateinfo->word_occurences[stateinfo->word_occurences_size++] = argv[i];
				continue;
			}
		} else if( exclude_files_flag ) {
			if( argv[i][0] == '-' ){
				exclude_files_flag = 0;
			}else{
				stateinfo->consumed_args[i] = OPT_consumed;
				stateinfo->exclude_files[stateinfo->exclude_files_size++] = argv[i];
				continue;
			}
		}
		/* END */

		// Is this argument an option?
		if( argv[i][0] == '-' ) {
			// mark the argument as being consumed.
			stateinfo->consumed_args[i] = OPT_consumed;

			if( strcmp_arg("-h") || strcmp_arg("-help") || strcmp_arg("--help") ){
				print_usage(argc, argv);
				return -1;
			} else if( strcmp_arg("-w") || strcmp_arg("--words") ) {
				// Handle words options
				if( stateinfo->words_flag == 0 ){
					opt_flag =1;
					stateinfo->words_flag = order_num++;
				}

			}else if( strcmp_arg("-l") || strcmp_arg("--lines") ) {
			 	// handle the lines options
				if( stateinfo->lines_flag == 0 ){
					opt_flag =1;
					stateinfo->lines_flag = order_num++;
				}

			} else if( strcmp_arg("-c") || strcmp_arg("--chars") ) {
				// Handle the character options
				if( stateinfo->chars_flag  == 0){
					opt_flag = 1;
					stateinfo->chars_flag = order_num++;
				}

			} else if( strcmp_arg("-C") || strcmp_arg("--max-chars") ) {
				// handle the max line lenght option
				if( stateinfo->max_chars_line_flag == 0 ) {
					opt_flag = 1;
					stateinfo->max_chars_line_flag = order_num++;
				}

			} else if( strcmp_arg("-W") || strcmp_arg("--max-words") ) {
				// Handle the max word lenght option
				if( stateinfo->max_words_line_flag == 0 ){
					opt_flag =1;
					stateinfo->max_words_line_flag = order_num++;
				}
			} else if( strcmp_arg("-v") || strcmp_arg("--version") ){
				print_version_info(stateinfo);		
				return -1;	
			} else if( strncmp(argv[i],"--padding",strlen("--padding")) == 0 ){
				// handling the padding option
				if( stateinfo->padding <= 0 ){
					if( sscanf(argv[i], "--padding=%d", &stateinfo->padding)  != 1 ){
						// get the error padding string.
						char error_padding[strlen(argv[i])+1];
						memset(error_padding, 0, strlen(argv[i]) +1);
						sscanf(argv[i],"--padding=%s",error_padding);

						print_error(stateinfo,"Invalid padding \"%s\".\n",error_padding);
						stateinfo->padding = 0;
					}
				}	

			} else if( strcmp_arg("--print-header") ) {
				// Handle the print format flag.	
				stateinfo->print_header_flag = 1;
			} else if ( strcmp_arg("--") ) {
				//This marks the end of parsing for options
				if( i <  argc-1) { 
					file_flag =1;
					for( i = i+1 ; i < argc; ++i){
						stateinfo->consumed_args[i] = OPT_normal;
					}
				}
				break;	
			} else if (strcmp_arg("--source-files")){
				source_flag =1;
			} else if( strcmp_arg("--word-occurences") ) {
				// Handle the counting of words occurences
				if( stateinfo->word_occurences_flag == 0 ){
					opt_flag =1;
					word_occurences_flag = 1;
					stateinfo->word_occurences_flag = order_num++;
				}
			} else if(strcmp_arg("--print-summary") ) {
				stateinfo->print_summary_flag =1;	
			} else if(strcmp_arg("-q") || strcmp_arg("--quiet") ){
				stateinfo->quiet_flag = 1;			
			} else if(strcmp_arg("--exclude") ) {
				if(stateinfo->exclude_files_flag == 0 ){
					stateinfo->exclude_files_flag = 1;
					exclude_files_flag = 1;
				}
			} else {
				struct stateinfo_t* si = stateinfo;
				print_error(si,"\'%s\' is not an option. Type %s -h for usage.\n", argv[i], argv[0]);
				continue;
			}


		}else{ // Not an option. Therefore a file.
			file_flag = 1;
			stateinfo->consumed_args[i] = OPT_normal;
		}
	}
	#undef strcmp_arg


	/* HANDLE THE DEFAULT CASE S*/
	// No input files, means take from stdin
	stateinfo->file_flag = file_flag;

	// Handle default padding.
	if( stateinfo->padding == 0 ) {
		stateinfo->padding = DEFAULT_PADDING;
	}


	// Default format ( lines, words, chars)
	if( opt_flag == 0 ){
		stateinfo->lines_flag = 1;
		stateinfo->words_flag =2;
		stateinfo->chars_flag = 3;
	}
	return 0;
}



int parse_sourcefile(struct wordcount_t* wc, struct stateinfo_t* state, FILE* fin){
	char* line = 0;
	unsigned linesize = 0;
	int rs = 0;
	struct wordcount_t summary;
	init_wordcount(&summary, state);

	FILE* file = 0;
	while( getline(&line,&linesize,fin) != -1 ) {
		line[strlen(line)-1] = 0; // cut the newline character.
		if( strcmp(line,"") == 0 ) { continue;} // don't parse empty lines.

		init_wordcount(wc, state);
		file = fopen(line,"r");
		if(!file){ print_error(state,"Unable to open file \"%s\".\n",line);continue;}

		rs = parsefile(wc,state,file);
		if(rs != 0){ print_error(state,"Unable to parse file.\"%s\".\n",line);}

		if( rs == 0 && state->print_summary_flag) {
			add_to_summary(&summary,wc,state);
		}

		print_information(wc,state,line);
		clear_wordcount(wc);
	}

	init_wordcount(wc,state);	
	add_to_summary(wc,&summary,state);
	free(line);
	return 0;
}


static void  count_occurences(char* word, struct stateinfo_t* state, struct wordcount_t* wc){
	int i = 0;
	for( i = 0 ; i < state->word_occurences_size; ++i){
		if( strcmp(state->word_occurences[i],word) == 0 ){
			wc->word_occurences_count[i] += 1;
			break;
		}
	}	
}
int parsefile(struct wordcount_t* wc_t,struct stateinfo_t* state,FILE* fin){
	int word_flag = -1;
	int line_word_count = 0;
	int line_char_count = 0;
	int c = 0;
	
	int word_buffer_pos = 0;
	char word_buffer[512]; // I am gueesing that this is reasonable size for a word.
	double overflowed_flag = 0;

	
	while( (c = fgetc(fin) ) != EOF ){
		wc_t->cc++;
		line_char_count++;
		if( iswhitespace(c) ){
			if(word_flag == 1 ){
				wc_t->wc++;
				line_word_count++;

				if( state->word_occurences_flag){
					word_buffer[word_buffer_pos] = 0;
					count_occurences(word_buffer, state, wc_t);
				}
			}
			word_flag = 0;
		}else if( c == '\n') {
			wc_t->lc++;
			if( word_flag == 1 ){
				wc_t->wc++;

				if( state->word_occurences_flag) {
					word_buffer[word_buffer_pos] = 0;
					count_occurences(word_buffer, state, wc_t);	
				}
			}

			if( line_word_count > wc_t->max_words_line ) { wc_t->max_words_line = line_word_count;}
			if( line_char_count > wc_t->max_chars_line ) { wc_t->max_chars_line = line_char_count;}
			line_word_count = 0;
			line_char_count = 0;
			word_flag = 0;
		}else {	

			// record the characters as they enter the word buffer.
			if(state->word_occurences_flag) {
				// reset the word buffer;
				if( word_flag == 0 ){
					if( overflowed_flag ) {
						print_error(state,"by %.0f characters.\n", overflowed_flag);
					}
					overflowed_flag = 0;
					word_buffer_pos = 0;
				}

				if( word_buffer_pos >= 512) { 
					if( overflowed_flag == 0 ) {
						print_error(state,"Overflow of word buffer "); 
						overflowed_flag++;
					}else{
						overflowed_flag++;
					}
				} else{
					word_buffer[word_buffer_pos++] = c;	
				}
			}

			word_flag = 1;

		}
	}

	return 0;
}

#endif
