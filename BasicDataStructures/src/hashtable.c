/**
 *	Author: Jordan Yu.
 *	Date: November 12th, 2012 Monday 9:31 pm.
 *
 *
 * This implmentation of the hashtable is using a linear probing method
 *
 * About use:
 *  - 	To construct a hashtable, you need:
 *  		hashfunction,
 *  		key comparator,
 *  		key Size,
 *  		elem Size,
 *  		item destroyer function
 * 	-	remove() does not return the elem, it will return bool success, failure.
 * 	-	find()  will return a pointer to the exact piece of memory holding elem.
 * 			so BE CAREFUL with it.
 * 	- 	get() is used to "get" a copy of the data held in the table.
 * 	-	Insert requires you to specify 3 parameters.
 * 			ht = pointer to hashtable.
 * 			key	= a pointer from which we copy the key value.
 * 			elem = a pointer from which we copy the elem value.
 * 	-	GENERAL use, 
 * 			insert( pass the address)
 * 			find/get ( dereference to get at elem);
 * 	- WHAT YOU NEED TO FREE
 * 			- Hashtable
 * 			- get() --> free item, free the elem held by item.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "hashtable.h"

void HashItem_constructor(HashItem* hi,
						  void* key,
						  void* elem,
						  int keySize,
						  int elemSize)
{
	if( hi == NULL){ return;}
	hi->flag = HASHTABLE_VACANT;
	hi->key = malloc(keySize);
	hi->elem = malloc(elemSize);
	hi->keySize = keySize;
	hi->elemSize = elemSize;

	memcpy(hi->key,key,keySize);
	memcpy(hi->elem,elem,elemSize);
}

void HashItem_destructor(HashItem* hi){
	if( hi == NULL ){return ;}
	free(hi->key);hi->key = 0;
	free(hi->elem);hi->elem = 0;
	hi->keySize = 0;
	hi->elemSize = 0;
	hi->flag = HASHTABLE_VACANT;
}

HashItem* HashItem_create(void* key,
						  void* elem,
						  int keySize,
						  int elemSize)
{
	HashItem* hi = malloc(sizeof(HashItem));
	HashItem_constructor(hi,key,elem,keySize,elemSize);
	return hi;
}

void HashItem_destroy(HashItem* hi){
	if( hi == NULL) {return ;}
	HashItem_destructor(hi);
	free(hi);
}














/**
 *	Hashtable Definition
 *
 *
 *
 *
 *
 *
 */
static void _Hashtable_reSizeTable(Hashtable* ht);
static HashItem* _Hashtable_findItem(Hashtable* ht, void* key,int* type);

static void* Hashtable_get(Hashtable* ht, void* key);
static int Hashtable_remove(Hashtable* ht,void* key);
static void* Hashtable_find(Hashtable* ht, void* key);
static int Hashtable_insert(Hashtable* ht,void* key, void* elem);



Hashtable* Hashtable_create(Hashtable_hashFunction f,
							Hashtable_keyCmp keycmp,
							int keySize,
							int elemSize,
							Hashtable_item_destroy iDestroy)
{

	Hashtable* ht = malloc(sizeof(Hashtable));
	if( ht == NULL) {return NULL;}
	ht->table = malloc( sizeof(HashItem) * (HASHTABLE_DEFAULT_SIZE));
	if( ht->table == NULL){free(ht);return NULL;}

	memset(ht->table,0,sizeof(HashItem)*HASHTABLE_DEFAULT_SIZE);/* Vacant */ 
	ht->tableSize = HASHTABLE_DEFAULT_SIZE;
	ht->keySize = keySize;
	ht->elemSize= elemSize;
	ht->size = 0;

	ht->hashFunction = f;
	ht->keyCmp = keycmp;
	ht->remove = &Hashtable_remove;
	ht->find = &Hashtable_find;
	ht->get = &Hashtable_get;
	ht->insert = &Hashtable_insert;
	ht->itemDestroy = iDestroy;
	return ht;
}
void Hashtable_destroy(Hashtable* ht){
	if( ht == NULL){return;}
	int i;
	for( i = 0; i < ht->tableSize; i++){
		if( ht->itemDestroy != NULL){
			ht->itemDestroy( (ht->table +i)->key, (ht->table+i)->elem);
		}
		HashItem_destructor(((ht->table)+i)) ;
	}
	free(ht->table);
	free(ht);
}



/* type 0 == VACANT item,
 * type 1 == OCCUPIED item and matching key.
 * type 2 == table is full.
 */
static HashItem* _Hashtable_findItem(Hashtable* ht, void* key,int* type){
	if( !ht || !key || !type ){return NULL;}
	HashItem* table = ht->table;
	int index = ht->hashFunction(key, ht->tableSize);
	int start = index;
	*type = 0;

	do{
		if( (table+index)->flag == HASHTABLE_VACANT){
			*type = 0;
			return (table+index);
		}else if( (table+index)->flag == HASHTABLE_OCCUPIED){
			if( ht->keyCmp((table+index)->key,key) == 0){
				*type = 1;
				return (table+index);
			}else{
				index = (index+1)%ht->tableSize;
			}
		}
	}while(index != start);
	*type = 2;
	return (table+index);
}

/* 1 good removal, 0 bad removal*/
static int Hashtable_remove(Hashtable* ht, void* key){
	if( !ht || !key ){return 0;}
	int type =0;
	HashItem*  target = _Hashtable_findItem(ht,key,&type);
	if(target == NULL) {return 0;}

	if(type == 1){
		if(ht->itemDestroy != NULL){
			ht->itemDestroy(target->key, target->elem);
		}
		HashItem_destructor(target);
		target->flag = HASHTABLE_VACANT;
		ht->size--;
		return 1;
	}else{
		return 0;
	}
}


/* returns a new piece of memory, so don't forget to free */
static void* Hashtable_get(Hashtable* ht, void* key){
	if( !ht || !key ) {return NULL;}
	int type =0;
	HashItem* target = _Hashtable_findItem(ht,key,&type);
	if( target == NULL) {return NULL;}

	switch(type){
		case(0):{return NULL;}break;
		case(1):{
				void* ret = malloc(target->elemSize);
				if( ret == NULL) {return NULL;}				
				memcpy(ret,target->elem,target->elemSize);
				return ret;	
		}break;
		case(2):{return NULL;}break;
		default:{return NULL;}
	}
}


/* be careful with this memory, it is actually held in the hashtable */
static void* Hashtable_find(Hashtable* ht, void* key){
	if( !ht || !key ) {return NULL;}
	int type =0;
	HashItem* target = _Hashtable_findItem(ht,key,&type);
	if( target == NULL) {return NULL;}

	switch(type){
		case(0):{return NULL;}break;
		case(1):{return target->elem;}break;
		case(2):{return NULL;}break;
		default:{return NULL;}
	}
}


/* 1 good return, 0  not inserted */
static int Hashtable_insert(Hashtable* ht,void* key,void* elem){
	if( !ht || !key || !elem ){return 0;}
	int type =0;
	HashItem* target = NULL;
	do{
		type =0;
		target = _Hashtable_findItem(ht,key,&type);
		if(target == NULL){return 0;}
	
		if( type == 0){ /* vacant */
			HashItem_constructor(target,key,elem,ht->keySize,ht->elemSize);
			target->flag = HASHTABLE_OCCUPIED;
			ht->size++;
			return 1;
		}else if(type == 1){/* occupied */
			break;
		}else if(type == 2){/* table is full */
			_Hashtable_reSizeTable(ht);
			continue;
		}else {
			break;
		}
	}while(1);

	return 0;
}


static void _Hashtable_reSizeTable(Hashtable* ht){
	if( !ht ){return;}
	HashItem* table =  ht->table;
	int tableSize = ht->tableSize;
	ht->table = malloc( sizeof(HashItem) * (ht->tableSize*2 +1)); 
	if( ht->table == NULL){ ht->table = table; return;}
	ht->tableSize = (ht->tableSize *2) +1;
	memset( ht->table, 0, ht->tableSize * sizeof(HashItem));

	int i =0;
	for( i =0; i < tableSize; i++){
		if( (table+i)->flag == HASHTABLE_OCCUPIED){
			ht->insert(ht,(table+i)->key,(table+i)->elem);
		}
		if( ht->itemDestroy != NULL){
			ht->itemDestroy( (table+i)->key, (table+i)->elem );
		}
		HashItem_destructor(table+i);
	}	
	free(table);
}


