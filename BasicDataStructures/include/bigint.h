#ifndef BIGINT_H
#define BIGINT_H

typedef struct bigint_t bigint;
struct bigint_t{
	char* number;
	int cap; /*we note that the cap includes ( strlen + '\0' + -ve buffer ) */


	bigint* (*copy)		(bigint* self);
	char* 	(*add) 		(bigint* self, bigint* num2);
	char* 	(*subtract) (bigint* self, bigint* num2);
	char* 	(*multiply) (bigint* self, bigint* num2);
	char* 	(*divide) 	(bigint* self, bigint* num2);
	int 	(*compare)	(bigint* self, bigint* num2);
	int 	(*set) 		(bigint* self, char* num);


	char*	(*add_c) 		(bigint* self, char* num);
	char*	(*subtract_c) 	(bigint* self, char* num);
	char*	(*multiply_c) 	(bigint* self, char* num);
	char*	(*divide_c)		(bigint* self, char* num);
	int		(*compare_c)	(bigint* self, char* num);

};


bigint* create_bigint(char* num);
void destroy_bigint(bigint* self);
#endif
