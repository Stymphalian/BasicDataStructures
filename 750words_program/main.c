#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structures.h"
#include "output.h"
#include "parser.h"
int run_entry(stateinfo_t*);

int main(int argc, char** argv){
	stateinfo_t state;
	if( parse_args(&state,argc,argv) == -1 ){return 0;}
	if( parse_config(&state,state.config) == -1) {clear_stateinfo_t(&state);return 0;}

	#ifdef DEBUG
		print_stateinfo(&state);
	#endif

   run_entry(&state);
   clear_stateinfo_t(&state);
	return 0;
}

int run_entry(stateinfo_t* state){
   //variables
   int new_flag = 0;

   // initialize entry
   entry_t entry;
   memset(&entry,0,sizeof(entry_t));
   strcpy(entry.entry,get_entry_name());
   strcpy(entry.author,state->author);
	strcpy(entry.path,state->dest);
   entry.file  = 0;

	// the absolute path to the file
	char abs_file[MAX_PATH + MAX_FILENAME];
	memset(abs_file,0,MAX_PATH +MAX_FILENAME);
	strcat(abs_file,entry.path);
	strcat(abs_file,entry.entry);
	
	// handle the rewrite flag
   if( file_exists(abs_file) ) {
      new_flag = 0;
      char* file_param = "a";
      if(state->rewrite_flag){
         new_flag = 1;
         file_param ="w";
      }
      entry.file = fopen(abs_file,file_param);
   }else{
      new_flag =1;
      entry.file = fopen(abs_file,"w");
      if(!entry.file) {
         print_error("Unable to create file '%s'\n",abs_file);
         return -1;
      }
   }

   // handle header info.
   if(new_flag && state->no_header_flag == 0) {
      print_header(state,&entry);
   }
   //close the file.
   fclose(entry.file);

   // if no editor is specified open up in notepad.
   if( state->editor_flag == 0 ){
      state->editor_flag = 1;
      strcpy(state->editor,"notepad");
   }

   // the system calls.   
   if( state->editor_flag  && state->count_flag == 0) {
      system_call("%s %s",state->editor,abs_file);
   }
   if( state->wc_flag) {
      system_call("%s %s",state->wc,abs_file);
   }

   //clean up the entry.
   memset(&entry,0,sizeof(entry_t));
   return 0;
}
