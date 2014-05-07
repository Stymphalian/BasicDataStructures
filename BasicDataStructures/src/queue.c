#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "queue.h"

static void* Queue_dequeue (Queue* self){
	if(self->front == self->back){ return NULL;}
	void* rs = self->queue + self->front*self->size;
	self->front = (self->front+1) % self->len;
	return rs;
}

static void Queue_enqueue (Queue* self, void* val){
	if( ((self->back+1) % self->len) == self->front){return;}
	char* value = (char*) val;
	int pos=0;
	do{
		*(self->queue + self->back*self->size + pos) = *(value+pos);
		pos++;
	}while( pos < self->size);
	self->back = (self->back + 1) % self->len;
}

static void* Queue_peek(Queue* self){
	if( self->front == self->back) {return NULL;}
	return self->queue + self->front;
}

static int Queue_isEmpty(Queue* self){
	return ( self->front == self->back);
}


Queue* Queue_create (unsigned int len, unsigned int size){
	Queue* q = malloc( sizeof(Queue));
	q->queue = malloc( sizeof(char)*(len+1)*size);
	q->front = 0;
	q->back = 0;
	q->len = len + 1;
	q->size = size;
	q->enqueue = &Queue_enqueue;
	q->dequeue = &Queue_dequeue;
	q->peek = &Queue_peek;
	q->isEmpty = &Queue_isEmpty;
	return q;
}

void Queue_destroy(Queue* self){
	if( !self ) {return; }
	free(self->queue);
	free(self);
}
