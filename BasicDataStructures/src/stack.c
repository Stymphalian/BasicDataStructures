#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stack.h"
static void* Stack_pop(Stack* self){
	if( self->top == self->stk) {return NULL;}
	self->top = self->top - self->size;
	return (self->top);
}

static void Stack_push(Stack* self, const void* val){
	if( self->top >=  self->stk + self->len*self->size) {return ;}
	const char* value = val;
	int i =0;
	for(i = 0; i < self->size; i++){
		*(self->top++) = *(value++);
	}
}

static void* Stack_peek(Stack* self){
	if( self == NULL ) { return NULL;}
	if( self->top == self->stk) {return NULL;}
	return (self->top - self->size);
}

static int Stack_isEmpty(Stack* self){
	if( self == NULL ) {return 1;}
	return ( self->top == self->stk);
}


Stack* Stack_create(unsigned int len, unsigned int size){
	Stack* stack = malloc(sizeof(Stack));
	stack->stk = malloc(size*len);
	stack->top = stack->stk;
	stack->len = len;
	stack->size = size;
	stack->pop = &Stack_pop;
	stack->push = &Stack_push;	
	stack->peek = &Stack_peek;
	stack->isEmpty = &Stack_isEmpty;
	return stack;
}

void Stack_destroy (Stack* self){
	if( self == NULL){return;}
	free(self->stk);
	free(self);
}



