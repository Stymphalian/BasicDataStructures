#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "md5.h"

static char* get_string_from_params(int argc, char** argv, int i);
int my_getline(char** string, unsigned int* size, FILE* file);

static void md_check(char* filename);
static char* md_string(char* string); /* null terminate the string */
static char* md_file(char* filename);
static char* md_text();

static void print_md5code(unsigned char* code, char* filename);
static void print_cmd(int argc, char** argv, FILE* stream);
static void printusage();


int main(int argc, char** argv){
	if( argc == 1){ printusage(); }

	int i;
	for(i = 1 ; i < argc; ++i){

		/* If it is an option */
		if(argv[i][0] == '-' ){
			/* print help message */
			if( strcmp(argv[i],"-h")  == 0 || strcmp(argv[i],"--help") == 0){
				printusage();

			/* Check all the files listed */
			}else if (strcmp(argv[i],"-c") == 0) {
				if( i+1 < argc){
					/* check only if a filename parameter is given */
					md_check(argv[i+1]);
				}else{
					/* if no paramter is given, print out an error message and the help*/
					print_cmd(argc,argv,stderr);
					fprintf(stderr," is not a valid command.\n");
					printusage();
				}
				return 0;	
			
			/* Get user input from stdin and calculate the md5 check sum */
			}else if( strcmp(argv[i], "-t") == 0){

				char* code =md_text();
				print_md5code( (unsigned char*) code, "");
				free(code);
				return 0;

			/* Use the provide string as the source message for the md5 check sum */
			}else if( argv[i][0] == '-' && argv[i][1] == 's'){

				/* obtain the string from the parameters after the -s */
				char* string = get_string_from_params(argc,argv,i);
				char* code = md_string(string);
			
				print_md5code((unsigned char*) code,string);
				free(code);	
				free(string);

				return 0;

			/* Handle the case in which an  invalide/unrecognized parameter option is given */
			}else{ 
				print_cmd(argc, argv, stderr);
				fprintf(stderr," is not a valid command.\n");
				printusage();
				return 0;
			}			

		}else{ /* else it is a filename */
			char* code = md_file(argv[i]);
			print_md5code((unsigned char*) code, argv[i]);
			free(code);
			return 0;	
		}
	}

	return 0;
}


/* parse the string from the paramerters */
char* get_string_from_params(int argc, char** argv, int i){

	/* calculate the length of the string from the parameters */
	int len = 0;
	int j;
	len += strlen(argv[i]);
	for(j = i+1; j < argc; ++j){
		len += strlen(argv[j]) + 1; /* +1 in order to handle ths space between parameters */
	}	


	len -= 2; /* -2 in order to avoid the -s in the parameter */
	char* string = malloc(sizeof(char) * (len+1));
	string[len] = '\0';
	int pos = 0;

	/* special case of handling the string immediately next to the -s  ( i.e  -sstring ) */
	memcpy(string+pos, argv[i]+2, strlen(argv[i])); /* +2 to avoid -s */
	pos += strlen(argv[i]) - 2;

	for( j = i+1; j < argc; ++j){
		string[pos++] = ' '; /* add the space in between each parameter */
		memcpy(string+pos, argv[j], strlen(argv[j]) );
		pos += strlen(argv[j]);
	}

	return string;
}


static char* md_text(){
	unsigned char buffer[64];
	MD_CTX context;
	UINT32 len = 0;

	memset(buffer, 0, 64);

	init_context(&context);
	while( (len = fread(buffer, 1, 64, stdin) ) ){
		if( len <64) {break;}
		update_context(&context,buffer);
	}
	update_final(&context,buffer,len);

	char* hashcode = malloc(sizeof(char)*16);
	memcpy(hashcode,context.state,16);
	return hashcode;	
}


static char* md_file(char* filename){
	FILE* file;
	unsigned char buffer[64];
	MD_CTX context;

	memset(buffer, 0, 64);


	file = fopen(filename, "rb");
	if( !file ){
		fprintf(stderr,"%s unable to open file\n", filename);
		return NULL;
	}else{
		UINT32 len =0;


		/* initialzie the context ( i.e A,B,C,D ) */
		init_context(&context);

		/* read in exactly 512 bits for each processing */
		while( (len = fread(buffer,1,64,file) ) ){
			/* if the input is less than the buffer size, 
			 * we know we are at the situation in which we are 
			 * at the end of the input */
			if( len < 64 ){break;} 


			/* update the context with the given buffer information */
			update_context(&context,buffer);
		}

		/* handle the final part of the input message.
		 * This includes the part of appending the padding 1/0s 
		 * as well as adding the 64-bit representation of the length */
		update_final(&context,buffer,len);


		/* save the 128-bit check sum into a variable */
		char* hashcode = malloc(sizeof(char) * 16);
		memcpy(hashcode,context.state, 16);
		return hashcode;
	}

	fclose(file);
	return NULL;
}

static char* md_string(char* string){
	if(!string){
		fprintf(stderr,"null string\n");
	}else{
		int length = strlen(string);

		MD_CTX context;
		unsigned char buffer[64];
		memset(buffer, 0, 64);
		int len = 0;
		int pos = 0;

		init_context(&context);
		for( len = 0 ; len < length ; ++len){
			buffer[pos++] = string[len];

			if( pos == 64) {
				update_context(&context,buffer);
				memset(buffer,0,64);
				pos = 0;
			}
		}
		update_final(&context,buffer,pos);

		char* code = malloc(sizeof(char)*16);
		memcpy(code, context.state, 16);
		return code;
	}

	return 0;
}

static void md_check(char* src_file){
	FILE* file = fopen(src_file,"r");

	if( !file ){
		fprintf(stderr,"%s unable to open.\n",src_file);
	}else{
		char* filename = 0;
		size_t size = 0;

		int len = 0;	
		char* code =0;

		while( (len = my_getline(&filename,&size,file) ) != -1 ){
			filename[len-1] = '\0';

			code = md_file(filename);				
			print_md5code((unsigned char*) code, filename);
			free(code);
		}	
		free(filename);
	}

	fclose(file);
	return;
}


static void print_md5code(unsigned char* hashcode, char* filename){
	if( !hashcode || !filename ){return ;}
	int i ;
	for( i = 0; i < 16; ++i){
		printf("%02x", hashcode[i]);
	}
	printf(" * (%s)\n", filename);
	return;
}

static void print_cmd(int argc, char** argv, FILE* stream){
	if(!stream){stream = stdout;}
	if(!argv){return ;}
	if(!*argv){return ;}

	int i = 0;
	for(i = 0; i < argc; ++i){
		fprintf(stream,"%s ", argv[i]);
	}
	return;
}


static void printusage(){
	printf("** USAGE ** Created by Jordan Yu\n");
	printf("Date: %s (%s)\n\n",__DATE__, __TIME__);

	printf("Usage: md5 [options] <filename>\n");

	printf("\nOptions:\n");
	printf("\t-h\t\tDisplay this help message and then exits.\n");
	printf("\t--help\t\tDisplay this help message and then exits.\n");
	printf("\t-sstring\tDetermine the md5 hashcode of the string.\n");
	printf("\t\t\tsurround the string in quotes to capture the entire string\n");
	printf("\t-c\t\tCalculate the md5s of the files listed in the file.\n");
	printf("\t-t\t\tGet input from stdin and calculate the md5 code.\n");
	printf("\t\t\tUse EOF to end user input.\n");
	printf("\n");
}
