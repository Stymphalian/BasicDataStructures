#ifndef STACK_H
#define STACK_H
typedef struct Stack_t Stack;
struct Stack_t {
	char* stk;
	char* top;
	unsigned int len;
	unsigned int size;

	void* (*pop) (Stack* self);
	void (*push) (Stack* self, const void* value);
	void* (*peek) (Stack* self);
	int (*isEmpty) (Stack* self);
};

Stack* Stack_create (unsigned int len, unsigned int size);
void Stack_destroy (Stack* self);

#endif
