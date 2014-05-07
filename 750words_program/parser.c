#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "misc.h"
#include "structures.h"
#include "output.h"
#include "inireader.h"


int parse_args(stateinfo_t* state,int argc,char** argv){
	if(!state || !argv ){return -1;}
	init_stateinfo_t(state,argc,argv);

	int i;
	int author_flag = 0;
	int editor_flag = 0;
	int wc_flag = 0;
	int config_flag = 0;
	int dest_flag = 0;

	#define strcmp_arg(arg) ( strcmp(argv[i],arg) == 0 )	
	for( i = 1; i < argc; ++i){
		// option parameters;
		if( author_flag ){
			state->consumed_args[i] = OPT_consumed;
			strncpy(state->author,argv[i],sizeof(state->author));
			state->author[sizeof(state->author)-1] = 0;
			author_flag=0;
			continue;
		}else if( editor_flag ){
			state->consumed_args[i] = OPT_consumed;
			strncpy(state->editor,argv[i],sizeof(state->editor));
			state->editor[sizeof(state->editor)-1] = 0;
			editor_flag =0;
			continue;
		}else if( wc_flag ){
			state->consumed_args[i] = OPT_consumed;
			strncpy(state->wc, argv[i], sizeof(state->wc));
			state->wc[sizeof(state->wc)-1]=0;
			wc_flag = 0;
			continue;
		}else if( config_flag ){
			state->consumed_args[i] = OPT_consumed;
			strncpy(state->config,argv[i], sizeof(state->config));
			state->config[sizeof(state->config)-1] =0;
			config_flag = 0;
			continue;
		} else if( dest_flag) {
			state->consumed_args[i] = OPT_consumed;
			strncpy(state->dest,argv[i], sizeof(state->dest));
			int len = strlen(state->dest);
			if( len < sizeof(state->dest) - 2) {
				state->dest[len++] = '\\';
				state->dest[len] = 0;
			}
			state->dest[sizeof(state->dest)-1] = 0;
			dest_flag = 0;
			continue;
		}

		// an option
		if( argv[i][0] == '-') {
			state->consumed_args[i] = OPT_consumed;

			if( strcmp_arg("-h") || strcmp_arg("--help") ){
				print_usage(argc,argv);			
				return -1;
			}else if(strcmp_arg("-v") || strcmp_arg("--version") ){
				print_version(state);
				return -1;
			}else if( strcmp_arg("-r") || strcmp_arg("--rewrite") ) {
				if( !state->rewrite_flag) {
					state->rewrite_flag =1;
				}	
			}else if( strcmp_arg("-a") || strcmp_arg("--author") ) {
				if(!state->author_flag ) {
					state->author_flag =1;
					author_flag = 1;
				}

			}else if( strcmp_arg("-e") || strcmp_arg("--editor") ) {
				if( !state->editor_flag) {
					state->editor_flag = 1;
					editor_flag =1;
				}

			}else if( strcmp_arg("-wc") || strcmp_arg("--word-count") ) {
				if( !wc_flag ) {
					state->wc_flag =1;
					wc_flag =1;
				}

			}else if( strcmp_arg("--no-header") ) {
				if(!state->no_header_flag) {
					state->no_header_flag =1;
				}

			}else if( strcmp_arg("--config") ){
				if(!state->config_flag) {
					state->config_flag =1;
					config_flag =1;
				}

			}else if( strcmp_arg("-c") || strcmp_arg("--count") ){
				state->count_flag = 1;	
			}else if( strcmp_arg("-d") || strcmp_arg("--dest")) {
				if( !state->dest_flag ){
					state->dest_flag = 1;
					dest_flag =1;
				}

			}	else {
				print_error("Not an option '%s'\n",argv[i] );
			}
		} else { // not an option
			// hmm... right now we don't handle any non options for the program
		}
	}
	#undef strcmp_arg

	return 0;	
}


int parse_config(stateinfo_t* state, const char* filename){
	if(!state || !filename ){return -1;}
	char* config_file = 0;
	if( state->config_flag == 0 ){
		state->config_flag = 1;
		config_file = "config.ini";
		strcpy(state->config,"config.ini");

	}else { 
		config_file = state->config;
  	}	

	//User the destination directory to find the path to the configuration file
	int len = strlen(state->dest) + strlen(config_file) +1;
	char abs_config_path[len];
	memset(abs_config_path,0,len*sizeof(char));
	strcat(abs_config_path,state->dest);
	strcat(abs_config_path,config_file);

	config_t* config = 0;
	config = create_config_t(abs_config_path);
	if(config == 0) {return 0;}


	while( config->hasNext(config) ){
		if( config->next(config) == 0 ){continue;}

		const config_item_t* item;
		item = config->getEntry(config);
		if( item == 0) { continue;}

		// check to make sure we are in the right section
		if( strcmp(item->section,"") != 0 ) { continue;}

		// find the netry with the right name
		if( strcmp(item->name, "editor") == 0 ){
			if(state->editor_flag == 1 ){continue;}
			strncpy(state->editor,item->value,MAX_PATH);	
			state->editor[MAX_PATH-1] =0;
			state->editor_flag = 1;
		}else if( strcmp(item->name,"wc") == 0 ){
			if(state->wc_flag == 1 ){continue;}
			strncpy(state->wc, item->value,MAX_PATH);
			state->wc[MAX_PATH-1]=0;
			state->wc_flag= 1;
		}else if( strcmp(item->name,"author") == 0 ){
			if(state->author_flag == 1 ){ continue;}
			strncpy(state->author,item->value, MAX_FILENAME);
			state->author[MAX_FILENAME-1]=0;
			state->author_flag =1;
		}else if( strcmp(item->name,"dest") == 0 ){
			if(state->dest_flag == 1 ){continue;}
			strncpy(state->dest,item->value,MAX_FILENAME);
			state->dest[MAX_FILENAME-1] = 0;
			state->dest_flag =1;
		}
	}

	config = destroy_config_t(config);
	return 0;
}

