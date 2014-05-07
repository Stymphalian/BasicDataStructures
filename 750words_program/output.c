#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "output.h"
#include "structures.h"

int print_error(const char* msg,...){
	va_list args;
	va_start(args,msg);
	fprintf(stderr,"Error:");
	vfprintf(stderr,msg,args);
	va_end(args);
	return 0;
}

int print_usage(int argc,char** argv){
	printf("Usage %s [options]*\n", argv[0]);
	printf("Options:\n");
	printf("\t-h,--help     -- Print this usage message.\n");
	printf("\t-v,--version  -- Print the version information.\n");
	printf("\t-r,--rewrite  -- Restart today's entry.\n");
	printf("\t-c,--count    -- Print the # of words, then exit.\n");
	printf("\t--no-header   -- Do not print the header in this entry.\n");
	printf("\t-a,--author [author]  -- Set the author for this entry.\n");
	printf("\t-e,--editor [editor]  -- Set the path of the text editor.\n");
	printf("\t-wc,--word-count [wc] -- set the path to the wordcount program.\n");
	printf("\t--config [file].ini   -- Use [file] config file. Default is 'config.ini'\n");
	printf("\t-d,--dest [path]      -- Specify a working directory for the program.\n");
	return 0;
}
int print_version(stateinfo_t* state){
	printf("%-13s: 750words Utility\n",state->argv[0]);
	printf("Version      : %.2f\n", VERSION);
	printf("Author       : %s\n","Jordan Yu");
	printf("Compile Date : %s %s\n", __DATE__,__TIME__);
	return 0;
}
int print_stateinfo(stateinfo_t* state){
	int i =0;

	printf("argc            = %d\n",state->argc);
	printf("argv            = ");

	for( i = 0;i < state->argc;++i){
		printf("%s,", state->argv[i]);
	}printf("\n");	

	printf("consumed_args 	= ");	
	for( i = 0; i < state->argc; ++i ){
		char* str = 0;
		switch(state->consumed_args[i] ){
			case(OPT_none): str="none";break;
			case(OPT_consumed): str="consumed";break;					
			default: str="wtf";
		}
		printf("%s ",str);
	}printf("\n");

	printf("rewrite_flag    = %d\n", state->rewrite_flag);
	printf("no_header_flag  = %d\n", state->no_header_flag);
	printf("author_flag     = %d\n", state->author_flag);
	printf("editor_flag     = %d\n", state->editor_flag);
	printf("wc_flag         = %d\n", state->wc_flag);
	printf("config_flag     = %d\n", state->config_flag);
	printf("count_flag      = %d\n", state->count_flag);
	printf("dest_flag       = %d\n", state->dest_flag);
	printf("author          = {%s}\n", state->author);
	printf("editor          = {%s}\n", state->editor);
	printf("wc              = {%s}\n", state->wc);
	printf("config          = {%s}\n", state->config);
	printf("dest            = {%s}\n", state->dest);

	return 0;
}

int print_header(stateinfo_t* state, entry_t* entry){
   char time_str[128];
   time_t raw_time;
	struct tm* time_info;
	time(&raw_time);
	time_info = localtime(&raw_time);
	strftime(time_str,128,"%Y-%m-%d %I:%M:%S %p", time_info);

   fprintf(entry->file,"Date: %s\n", time_str);
   fprintf(entry->file,"%s\n\n\n",entry->author);
   return 1;
}

