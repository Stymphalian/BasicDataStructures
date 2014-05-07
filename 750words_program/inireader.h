/* Alright here is how this works.
 *
 * 1) create a config_t structure with create_config_t(filename)
 * 2) use hasNext() and next() to iterate through every valid entry in the config file.
 * 3) access the values section,name,value with the appropriate get* calls.
 * 4) destroy config_t with destroy_config_t();
 *
 * Details:
 * 	name=value   ** blank sections are treated as ""
 * 	[section]	 
 * 	name=value
 * 	;comment text
 *						 ** blank lines are skipped
 *		name=value
 *
 */


#ifndef INIREADER_H
#define INIREADER_H
#include <stdio.h>
typedef struct config_item_t config_item_t;
struct config_item_t{
	const char* section;
	const char* name;
	const char* value;
};

int set_config_item_t_section(config_item_t*, char*);
int set_config_item_t_name(config_item_t*, char*);
int set_config_item_t_value(config_item_t*, char*);
int set_config_item_t(config_item_t* c, char* s, char* n, char* v);
int clear_config_item_t(config_item_t* self);
void print_config_item_t(const config_item_t* item);

typedef struct config_t config_t;
struct config_t {
	// data
	FILE* file;
	const char* filename;
	config_item_t entry;

	// methods	
	int (*rewind)  (config_t* self);
	int (*hasNext) (config_t* self);	
	int (*next)    (config_t* self);

	// getter methods
	const config_item_t* (*getEntry) (config_t* self);
	const char* (*getSection) (config_t* self);
	const char* (*getName)  (config_t* self);
	const char* (*getValue) (config_t* self);

};

config_t* destroy_config_t(config_t* self);
config_t* create_config_t(const char* filename);
void print_config_t(const config_t*);
#endif
