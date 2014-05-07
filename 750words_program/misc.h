#ifndef MISC_H
#define MISC_H

#define MAX_FILENAME 256
#define MAX_PATH 1024
#define VERSION 1.21
enum { OPT_none,OPT_consumed };

void system_call(const char* msg, ...);
const char* get_entry_name();
int file_exists(const char* filename);

#endif
