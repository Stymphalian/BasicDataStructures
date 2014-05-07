#ifndef STRUCTURES_HH
#define STRUCTURES_HH

// structures and their constructor/destructors
struct wordcount_t{
	unsigned wc;
	unsigned cc;
	unsigned lc;
	unsigned max_words_line;
	unsigned max_chars_line;
	int* word_occurences_count;
};

struct stateinfo_t {
	int words_flag;
	int chars_flag;
	int lines_flag;
	int max_words_line_flag;
	int max_chars_line_flag;

	int word_occurences_flag;
	char** word_occurences;
	int word_occurences_size;
	
	int exclude_files_flag;
	char** exclude_files;
	int exclude_files_size;
	
	int padding;
	int print_header_flag;
	int print_summary_flag;
	int quiet_flag;
	int file_flag;
	int argc;
	char** argv;
	int* consumed_args;
};


void init_wordcount(struct wordcount_t* wc, struct stateinfo_t* state);
void clear_wordcount(struct wordcount_t* wc);
void init_stateinfo(struct stateinfo_t* si, int argc, char** argv);
void destroy_stateinfo(struct stateinfo_t* si);
int add_to_summary(struct wordcount_t* summary, struct wordcount_t* wc,struct stateinfo_t* si);
#endif
