#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "misc.h"

const char* get_entry_name(){
	static char entry_name[32];
	time_t raw_time;
	struct tm* time_info;
	time(&raw_time);
	time_info = localtime(&raw_time);
	strftime(entry_name, 32,"%Y-%m-%d", time_info);
	return entry_name;
}

int file_exists(const char* filename){
	FILE* fin = fopen(filename,"r");
	if(fin){
		fclose(fin);
		return 1;
	}else{
		return 0;
	}
}


void system_call(const char* msg, ...) {
	char buffer[1024];
	memset(buffer, 0, 1024);
	va_list args;
	va_start(args, msg);

	vsprintf(buffer,msg,args);
	system(buffer);

	va_end(args);
}


