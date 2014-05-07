#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "misc.h"
#include "structures.h"
#include "output.h"
#include "parser.h"

int main ( int argc, char** argv) {
	// begin the parsing all the files.
	int i =0;
	int rs = 0;
	FILE* fin = 0;
	struct stateinfo_t stateinfo;
	struct wordcount_t wc_t;
	struct wordcount_t summary_wc;

	// This is to correct a bug,which was causing a seg fault.
	memset(&stateinfo,0, sizeof(struct stateinfo_t));
	memset(&wc_t, 0,sizeof(struct wordcount_t));
	memset(&summary_wc,0,sizeof(struct wordcount_t));	

	do {
		// parse all the command line options	
		init_stateinfo(&stateinfo,argc, argv);
		if( parseargs(&stateinfo,argc,argv) == -1 ) {break;}
		init_wordcount(&summary_wc,&stateinfo);

		// If not input files were passed in, use STDIN
		if( stateinfo.file_flag == 0 ){
			// parse the file
			init_wordcount(&wc_t,&stateinfo);
			rs = parsefile(&wc_t,&stateinfo,stdin);

			// print the information
			if( stateinfo.print_summary_flag && rs == 0){
				add_to_summary(&summary_wc,&wc_t,&stateinfo);
			}
			if( stateinfo.print_header_flag) {
				print_header(&stateinfo);
			}
			print_information(&wc_t,&stateinfo,"stdin");

			// clean up
			clear_wordcount(&wc_t);
		}else{ // Parse through each of the input files.

			// print the format header.
			if( stateinfo.print_header_flag == 1 ){
				print_header(&stateinfo);
			}	

			// parse through each argument looking for input files.
			for( i = 1; i < argc; ++i) {
				// skip if the argument has already been consumed.
				if( stateinfo.consumed_args[i] == OPT_consumed ){ continue;}

				// check to see if the file is one of our exclude files.
				if( stateinfo.exclude_files_flag ) {
					char** list = stateinfo.exclude_files;
					int size = stateinfo.exclude_files_size;
					if( str_find_array(argv[i],list,size)){ continue;}
				}

				// Begine the parsing.
				init_wordcount(&wc_t,&stateinfo);	
	
				fin = fopen(argv[i], "r");
				if( !fin) {
					print_error(&stateinfo,"Cannot open file \"%s\"\n", argv[i]);
					continue;
				}
	
				// check if a source file or a normal file.
				if( stateinfo.consumed_args[i] == OPT_normal) {	
					rs = parsefile(&wc_t,&stateinfo,fin);
					if( rs != 0 ){ print_error(&stateinfo,"Unable to parse file \"%s\"\n",argv[i]); }
					print_information(&wc_t,&stateinfo,argv[i]);
				}else if (stateinfo.consumed_args[i] == OPT_source){
					// ** NOTE **, the parse_sourvefile will leave the summary wordcount information
					// in wc_t at the end of its runs.
					rs = parse_sourcefile(&wc_t,&stateinfo,fin);								
					if( rs != 0 ){ print_error(&stateinfo,"Unable to parse src file \"%s\"\n",argv[i]);}
				}

	
				// if the flag is set and the result from the previous parsing was successful.
				// Then we can add it to the summary wordcount_t
				if( stateinfo.print_summary_flag && rs==0){
					add_to_summary(&summary_wc, &wc_t,&stateinfo);
				}
			

				fclose(fin);
				clear_wordcount(&wc_t);
			}

		}
		if( stateinfo.print_summary_flag ){
			print_summary(&summary_wc,&stateinfo);
		}

	} while(0);

	clear_wordcount(&wc_t);
	clear_wordcount(&summary_wc);
	destroy_stateinfo(&stateinfo);
	return 0;
}
