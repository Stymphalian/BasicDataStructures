#include "globals.h"
#include "bnode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>




int bnode_destructor(bnode* self){
	if(!self) {return 0;}		
	free(self->value);
	free(self->key);
	return 1;
}

int bnode_init(bnode* self,
			void* key, void* value,
			int keySize, int valSize,
			bnode* parent, bnode* left,bnode* right)
{
	if(self == '\0' ) return 0;

	self->value = value;
	self->key = key;
	self->parent = parent;
	self->left = left;
	self->right = right;
	self->keySize = keySize;
	self->valSize = valSize;
	return 1;	
}



bnode* bnode_create(void* k,void* v,int kS,int vS,bnode* p,bnode* l,bnode* r){
	bnode* node = malloc(sizeof(bnode));
	if(!node){return NULL;}
	node->key = malloc(kS);
	node->value = malloc(vS);
	if(!node->key || !node->value){
		free(node->key);
		free(node->value);
		return NULL;
	}
	if(k){memcpy(node->key,k,kS);}
	else{node->key = 0;}

	if(v){memcpy(node->value,v,vS);}
	else{node->value = NULL;}

	bnode_init(node,node->key,node->value,kS,vS,p,l,r);
	return node;
}


int bnode_destroy(bnode* self){
	if(!self){return 0;}
	bnode_destructor(self);
	memset(self, 0, sizeof(bnode));
	free(self);
	return 1;
}
