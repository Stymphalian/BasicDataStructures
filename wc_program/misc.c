#ifndef MISC_H
#define MISC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"

char* get_padding_string(int padding, char type){
	static char str[16];
	memset(str,0,sizeof(str));

	sprintf(str,"%%%d%c",padding,type);
	str[31]=0; // just make sure we dont' overflow
	return str;
}

int iswhitespace(char c){
	switch(c){
		case(' '):
		case('\t'):return 1;
	}
	return 0;
}

int getline (char** line, unsigned* lineSize, FILE* file){
	if( line == NULL || file == NULL ) { return -1;}	
	if( *line == NULL || *lineSize == 0 ){
		if( *lineSize == 0 ){
			/*int defaultSize = 25;*/
			*lineSize = (size_t) 25;
			free(*line);
		}
		*line = malloc(sizeof(char)* (*lineSize ));
		if(!(*line)) { return -1; }
	}

	int count =0;
	char c =0;

	while( (c = fgetc(file)) != EOF) {
		if( count >= (*lineSize)-1 ){
			(*line) = realloc((*line),sizeof(char)* ((*lineSize) *2));
			if(!(*line)) {return -1;}
			*lineSize =  (*lineSize)*2;	
		}	
		(*line)[count++]=c;
		if( c == '\n') {break;}	
	}
	(*line)[count] = '\0';
	
	if(count == 0){ return -1;}	
	else{ return count;}
}

int str_find_array(char* file, char** list, int size) {
	if( !file || !list ) {return 0;}
	int i = 0;
	for( i = 0; i < size; ++i) {
		if( strcmp(file,list[i]) == 0 ){
			return 1;
		}
	}
	return 0;
}


#endif
