#include<stdio.h>
#include<stdlib.h>


/* 
 * Read up to and including the newline character.
 * Auto appends the null terminating character.
 * 
 *
 *
 * char** line // the line to put the text into
 * size_t*  lineSize // the amount of memory available in lin
 * FILE* file file // the file to get the line from
 * RETURN : number of chars read(not including terminating char),
 * 			 or -1 if there was an error.
 */


int my_getline (char** line,unsigned int* lineSize, FILE* file){
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
