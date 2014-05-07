#ifndef LIST_H
#define LIST_H



/**
 * USE:
 *
 *	don't forget to free the memory from a get()
 *	whatever you insert, the list will make a copy.
 *	insert -- pass address.
 *	find/get -- dereference from your "type"
 *
 */

typedef void list_elem_destroy (void* elem);
typedef struct List_t List;
struct List_t{
	void* container;
	int containerSize;
	int typeSize;

	int size;
	int (*remove)(List* self,int pos);
	int (*insert)(List* self,void* elem,int pos);
	void* (*find)(List* self,int pos);
	void* (*get)(List* self,int pos);
	void (*eDestroy)(void* elem);
};







#endif
