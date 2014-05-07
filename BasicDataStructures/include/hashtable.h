#ifndef HASHTABLE_H
#define HASHTABLE_H


#define HASHTABLE_VACANT 0
#define HASHTABLE_OCCUPIED 1
#define HASHTABLE_DEFAULT_SIZE 17

typedef struct HashItem_t HashItem;
struct HashItem_t{
	int flag; /* 0 VACANT, 1 OCCUPIED */
	void* key;
	void* elem;
	int keySize;
	int elemSize;
};

void HashItem_destructor(HashItem* hi);
void HashItem_constructor(HashItem* hi,
						  void* key,
						  void* elem,
						  int keySize,
						  int elemSize);
void HashItem_destroy(HashItem* hi);
HashItem* HashItem_create(void* key,
						  void* elem,
						  int keysize,
						  int elemSize);



typedef void (*Hashtable_item_destroy)(void* key, void* elem);
typedef int (*Hashtable_hashFunction) (void*,int);
typedef int (*Hashtable_keyCmp)(void* k1,void*k2);
typedef struct Hashtable_t Hashtable;
struct Hashtable_t{
	HashItem* table;
	int tableSize;

	int keySize;
	int elemSize;
	int size;

	int(*hashFunction) (void* key, int tableSize);
	int(*keyCmp) (void* key1, void* key2);
	int (*remove)(Hashtable* ht,void* key);
	void* (*find) (Hashtable* ht,void* key);
	void* (*get) (Hashtable* ht,void* key);
	int (*insert) (Hashtable* ht,void* key,void* elem);	
	void (*itemDestroy) (void* key, void* elem);
};

Hashtable* Hashtable_create(Hashtable_hashFunction f,
							Hashtable_keyCmp k,
							int keySize,
							int elemSize,
							Hashtable_item_destroy iDestroy);
void Hashtable_destroy(Hashtable* h);




#endif
