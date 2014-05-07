#ifndef BINARYTREE_H
#define BINARYTREE_H

#include "bnode.h"
#include "Stack.h"

/* declaring the binary tree */



typedef int binarytree_keyCmp(void* key1, void* key2);
typedef int binarytree_itemDestroy(void* key, void* elem);

typedef struct binarytree_t binarytree;
struct binarytree_t{
	bnode* root;
	unsigned int size;


	int keySize;
	int valSize;
	int (*keyCmp) (void* key1, void* key2);
	int (*itemDestroy) (void* key, void* elem);


	int (*height) (binarytree* tree);
	void* (*traverse) ( binarytree* tree, int* len, int type);
	int (*insert) (binarytree* tree,void* val,void* key);
	int (*remove) (binarytree* tree, void* key);
	void* (*find) (binarytree* tree, void* key);	
	void* (*get) (binarytree* tree, void * key);
};

int binarytree_destroy(binarytree* tree);
binarytree* binarytree_create(bnode* root, binarytree* left, binarytree* right,
				int keySize, int elemSize, 
				binarytree_keyCmp keyCmp,
				binarytree_itemDestroy iDestroy);

#endif
