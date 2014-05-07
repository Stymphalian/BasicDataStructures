#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue_t Queue;
struct queue_t{
	char* queue;
	unsigned int front;
	unsigned int back;
	unsigned int len;
	unsigned int size;

	void  (*enqueue) (Queue* q, void* value);
	void* (*dequeue) (Queue* q);
	void* (*peek) (Queue* q);
	int (*isEmpty) (Queue* q);
};

Queue* Queue_create(unsigned int len, unsigned int size);
void Queue_destroy(Queue* self);

#endif
