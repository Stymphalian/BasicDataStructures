#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "list.h"
#include "linked_list.h"

typedef struct llnode_t llnode;
struct llnode_t{
	void* elem;
	size_t elemSize;

	llnode* next;
	llnode* prev;
};
static void llnode_constructor(llnode* s, void* e,int eS,llnode* n,llnode* p){
	if( !s  || !e){return;}
	s->elem = malloc(eS);
	memcpy(s->elem, e, eS);
	s->elemSize= eS;
	s->next = n;
	s->prev = p;
}

static void llnode_destructor(llnode* self){
	if(!self) {return;}
	free(self->elem);
	self->elem = 0;
	self->elemSize = 0;
	self->next = 0;
	self->prev = 0;
}
static llnode* llnode_create(void* e,int eS,llnode* n,llnode* p){
	if(!e){return NULL;}
	llnode* node = malloc(sizeof( llnode));
	llnode_constructor(node,e,eS,n,p);
	return node;

}
static void llnode_destroy(llnode* self){
	if(!self){return;}
	llnode_destructor(self);
	memset(self,0, sizeof(llnode));
	free(self);	
}



typedef struct{
	llnode* head;
	llnode* tail;
}listStartNode;

static llnode* linked_list_findNode(List* self, int pos);
static int linked_list_insertNodeBefore(llnode* current, llnode* newNode);

static int linked_list_remove(List* self, int pos);
static int linked_list_insert(List* self, void* elem,int pos);
static void* linked_list_find(List* self, int pos);
static void* linked_list_get(List* self, int pos);



List* linked_list_create(int typeSize,list_elem_destroy eDestroy){
	List* list = malloc(sizeof(List));
	if(list == 0) {return NULL;}

	list->container = malloc(sizeof(listStartNode));
	list->containerSize = sizeof(listStartNode);
	list->typeSize = typeSize;

	if( list->container == 0){return NULL;}
	listStartNode* lsn = list->container;
	lsn->head = calloc(1,sizeof(llnode));
	lsn->tail = calloc(1,sizeof(llnode));
	lsn->head->next = lsn->tail; lsn->head->prev = NULL;
	lsn->tail->next = NULL     ; lsn->tail->prev = lsn->head;

	list->size = 0;
	list->remove = &linked_list_remove;
	list->insert = &linked_list_insert;
	list->find   = &linked_list_find;
	list->get = &linked_list_get;
	list->eDestroy = eDestroy;
	return list;
}

void linked_list_destroy(List* self){
	if( self == 0){return;}

	listStartNode* lsn = self->container;
	llnode* next = NULL;
	llnode* current = lsn->head;
	int count = 0;
	while(current != NULL){
		count++;
		next = current->next;
		if(self->eDestroy != NULL){
			self->eDestroy(current->elem);
		}
		llnode_destroy(current);
		current = next;
	}

	if( count != self->size +2){
		printf("linked_list_destroy missed some frees\n");
	}
	lsn->head = NULL;
	lsn->tail = NULL;
	free(self->container);
	memset(self,0, sizeof(List));
	free(self);	
}


static llnode* linked_list_findNode(List* self,int pos){
	if( !self ){return NULL;}
	if( pos < 0 || pos >= self->size+1){ return NULL;}
	listStartNode* lsn = self->container;
	llnode* head = lsn->head;
	llnode* tail = lsn->tail;
	if( !head || !tail ){return NULL;}
	

	int i;
	llnode* current = lsn->head->next;
	for( i = 0; i < pos; i++){
		current = current->next;
		if( current == NULL){return NULL;}
	}
	return current;
}

/* 1 for success, 0 for failure */
static int linked_list_remove(List* self, int pos){
	if(!self){return 0;}
	if( pos <0 || pos >= self->size){return 0;}
	if( self->size == 0){return 0;}
	listStartNode* lsn = self->container;
	llnode* head = lsn->head;
	llnode* tail = lsn->tail;
	if( !lsn->head || !lsn->tail ){return 0;}

	llnode* target = linked_list_findNode(self,pos);
	if( target == NULL ){return 0;}
	if( target == head || target == tail ){return 0;}

	target->next->prev = target->prev;
	target->prev->next =  target->next;
	if( self->eDestroy != NULL){
		self->eDestroy(target->elem);
	}
	llnode_destroy(target);
	self->size--;

	return 1;
}


static int linked_list_insertNodeBefore(llnode* current, llnode* newNode){
	if(!current || !newNode){return 0;}
	if(current->prev == NULL ) {return 0;}

	current->prev->next = newNode;
	newNode->next = current;
	newNode->prev = current->prev;
	current->prev = newNode;
	return 1;

}

static int linked_list_insert(List* self, void* elem,int pos){
	if( !self  ){return 0;}
	if( pos < 0 || pos > self->size){return 0;}

	llnode* target = linked_list_findNode(self,pos);
	if( target == NULL){return 0;}
	llnode* newNode = llnode_create(elem,self->typeSize,NULL,NULL);

	if( linked_list_insertNodeBefore(target,newNode)){self->size++;return 1;}
	else{ llnode_destroy(newNode);return 0;}
}

/* don't forget to free the memory */
static void* linked_list_get(List* self, int pos){
	if( !self  ){return NULL;}
	if( pos < 0 || pos >= self->size){return NULL;}

	llnode* current = linked_list_findNode(self,pos);
	if( current == NULL) {return NULL;}

	void* ret = malloc(current->elemSize);
	if( ret == NULL){return NULL;}
	memcpy(ret,current->elem, current->elemSize);
	return ret;
}

/* peeks at the element, but you don't have a copy yourself */
static void* linked_list_find(List* self, int pos){
	if( !self  ){return NULL;}
	if( pos < 0 || pos >= self->size){return NULL;}

	llnode* current = linked_list_findNode(self,pos);
	if( current == NULL) {return NULL;}
	return current->elem;
}
