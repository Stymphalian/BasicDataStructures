#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "inireader.h"

/* config_item_t */

int set_config_item_t_section(config_item_t* item,char* s){
	if( !item) {return 0;}
	if(item->section != 0 ){free((char*)item->section);}
	char* _section = (s==0) ? "" : s;
	char* section = malloc(strlen(_section)+1);
	if(!section) {return 0;}
	memcpy(section,_section,strlen(_section)+1);
	item->section = section;
	return 1;
}

int set_config_item_t_name(config_item_t* item,char* s){
	if( !item) {return 0;}
	if(item->name != 0 ){free((char*)item->name);}
	char* _name = (s==0) ? "" : s;
	char* name = malloc(strlen(_name)+1);
	if(!name) {return 0;}
	memcpy(name,_name,strlen(_name)+1);
	item->name = name;
	return 1;
}

int set_config_item_t_value(config_item_t* item,char* s){
	if( !item) {return 0;}
	if(item->value != 0 ){free((char*)item->value);}
	char* _value = (s==0) ? "" : s;
	char* value = malloc(strlen(_value)+1);
	if(!value) {return 0;}
	memcpy(value,_value,strlen(_value)+1);
	item->value = value;
	return 1;
}

int set_config_item_t(config_item_t* item, char* s, char* n, char* v){
	if( !item) {return 0;}
	int rs = 0;
	rs += set_config_item_t_section(item,s);
	rs += set_config_item_t_name(item,n);
	rs += set_config_item_t_value(item,v);
	return( rs== 3);
}

int clear_config_item_t(config_item_t* item){
	if( !item) {return 0;}
	free((char*)item->section);
	free((char*)item->name);
	free((char*)item->value);
	return 1;
}

void print_config_item_t(const config_item_t* item){
	if(!item) {return;}
	printf("[%s],%s=%s\n",item->section,item->name,item->value);
}


/*  config_t */

static int getline(char** line, unsigned* linesize,FILE* file);
static int iswhitespace(char c);
static int trimFrontStr(const char* str);
static const char* getExtension(const char* filename);
static int isLineValid(const char* line);

static int _rewind (config_t* self);
static int _hasNext(config_t* self);
static int _next(config_t* self);
static const config_item_t* _getEntry(config_t* self);
static const char* _getSection(config_t* self);
static const char* _getName(config_t* self);
static const char* _getValue(config_t* self);

config_t* destroy_config_t(config_t* self){
	if(!self) {return 0;}
	if( self->file  != 0 ){
		fclose(self->file);
	}
	free((char*) self->filename);
	clear_config_item_t(&self->entry);
	memset(self,0, sizeof(config_t));
	return 0;
}

config_t* create_config_t(const char* filename){
	if(!filename) {return 0;}
	if( strcmp(getExtension(filename),"ini") != 0 ) {return 0;}

	config_t* config = malloc(sizeof(config_t));
	memset(config,0,sizeof(config_t));
	memset(&config->entry,0,sizeof(config_item_t));


	do {
		FILE* fin = fopen(filename,"r");
		if(!fin) {fprintf(stderr,"Unable to open file '%s'\n",filename);break;}

		char* fname = malloc(strlen(filename) +1 );
		if(!fname) {break;}
		memcpy(fname,filename,strlen(filename)+1);

		config->file				= fin;
		config->filename 			= fname;
		set_config_item_t(&config->entry,"","","");
		config->rewind 			= _rewind;
		config->hasNext 			= _hasNext;
		config->next 				= _next;
		config->getEntry			= _getEntry;	
		config->getSection 		= _getSection;
		config->getName 			= _getName;
		config->getValue			= _getValue;

		return config;

	} while(0);

	destroy_config_t(config);
	return 0;
}

void print_config_t(const  config_t* config){
	printf("file			= %d\n",config->file);	
	printf("filename		= %s\n",config->filename);
	printf("entry			= ");
	print_config_item_t(&config->entry);
}


static int _rewind (config_t* self){
	if(!self) {return 0;}
	rewind(self->file);	
	return 1;
}

static int _hasNext(config_t* self){
	if(!self) {return 0;}

	long int before = ftell(self->file);
	char* line = 0;
	unsigned linesize = 0;
	int len = 0;

	int rs = 0;
	int found = 0;
	while( (len = getline(&line,&linesize,self->file ) ) != -1 ){
		if( line[len-1] == '\n' ) { line[len-1] = 0;}

		rs = isLineValid(line);
		if(rs == 0 ){continue;}
		if(rs == 1){continue;}
		if(rs == 2){found = 1;break;}
	}	

	free(line);
	fseek(self->file,before, SEEK_SET);
	return found;
}

int extractSection(char* line, char* section){
	char* open = strstr(line,"[");
	char* end = strstr(line,"]");
	memcpy(section,open+1,end-(open+1));
	section[end-(open+1)] = 0;
	return 1;
}
int extractName(char* line, char* name){
	char* equals = strstr(line,"=");
	memcpy(name,line, equals-line);
	name[equals-line] = 0;
	return 1;
}

int extractValue(char* line, char* value){
	char* equals = strstr(line,"=");
	char* end = line + strlen(line);

	memcpy(value,equals+1,end-(equals+1));
	value[end-(equals+1)] =0;
	return 1;
}


static int _next(config_t* self){
	if(!self) {return 0;}

	char* line = 0;
	unsigned linesize = 0;
	int len = 0;

	int rs = 0;
	int found = 0;
	while( (len = getline(&line,&linesize,self->file) != -1 )){
		if( line[strlen(line)-1] == '\n' ) {line[strlen(line)-1] = 0;}

		rs = isLineValid(line);
		if (rs == 0 ){continue;}
		if( rs == 1 ){
			char section[strlen(line)+1];
			extractSection(line,section);
			set_config_item_t_section(&self->entry,section);
			continue;
		}
		if( rs == 2){
			char name[strlen(line)+1];
			char value[strlen(line)+1];
			extractName(line,name);
			extractValue(line,value);
			set_config_item_t_name(&self->entry,name);
			set_config_item_t_value(&self->entry,value);

			found =1;
		  	break; 
		}
	}
	free(line);
	return found;
}

static const config_item_t* _getEntry(config_t* self){
	if(!self) {return 0;}
	return &self->entry;
}
static const char* _getSection(config_t* self){
	if(!self) {return 0;}
	return self->entry.section;
}
static const char* _getName(config_t* self){
	if(!self) {return 0;}
	return self->entry.name;
}
static const char* _getValue(config_t* self){
	if(!self) {return 0;}
	return self->entry.value;

}





/* Misc */
static int getline(char** line,unsigned* lineSize, FILE* file){
	if( line == NULL || file == NULL ) { return -1;}	
	if( *line == NULL || *lineSize == 0 ){
		if( *lineSize == 0 ){
			/*int defaultSize = 25;*/
			*lineSize = (size_t) 32;
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

static int iswhitespace(char c){
	switch(c) {
		case(' '):
		case('\t'):
		case('\n'):return 1;
		default: return 0;
	}
	return 0;
}
static int trimFrontStr(const char* str){
	if(!str) {return -1;}
	int i = 0;
	int len = strlen(str);
	while( iswhitespace(str[i]) && i < len ){
		++i;
	}

	if( i == len ) {return -1;} // the entire string was whitespace;
	return i;
}

static const char* getExtension(const char* filename){
	static char null_str[4] = "";
	if(!filename){return null_str;}

	int i = 0;
	int pos_of_period = -1;
	for( i = strlen(filename) -1 ; i >= 0 ; --i ){
		if( filename[i] == '.' ){
			pos_of_period = i;
			break;
		}
	}

	if( pos_of_period < 0 ){return null_str;}
	if( pos_of_period == strlen(filename)-1) {return null_str;}
	pos_of_period++;
	return filename+pos_of_period;
}

static int isLineValid(const char* line) {
	if(!line) {return 0;}

	int pos =  trimFrontStr(line);
	const char* trim = line;
	if( pos != -1 ){ trim = line + pos;}

	if( trim[0] == ';') { return 0;}	
	if( trim[0] == '\n'){ return 0;}
	if( trim[0] == EOF ){ return 0;}
	if( trim[0] == '=' ){ return 0;}

	// section header  OR name-value pair
	if( trim[0] == '[') {
		//handle section header
		char* end_bracket = strstr(trim,"]");
		if( end_bracket == 0) { return 0;}
		return 1;
	} else {
		//handle the name=value pair
		char* equals = strstr(line,"=");
		if(equals == 0 ){return 0;}
		return 2;
	}
	return 0;
}

