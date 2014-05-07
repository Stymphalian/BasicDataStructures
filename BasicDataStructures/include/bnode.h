#ifndef BNODE_H
#define BNODE_H


typedef struct b_node_t bnode;
struct b_node_t{
	void* value;
	void* key;

	int valSize;
	int keySize;
	bnode* parent;
	bnode* left;
	bnode* right;
};



/*self,key,value,keySize,valueSize,parent, left,right */
int bnode_init(bnode*,void*, void*,int,int,bnode*,bnode*, bnode*);
int bnode_destructor(bnode* self);

/* key,value,keySize,valueSize,parent, left,right */
bnode* bnode_create(void*,void*,int,int,bnode*,bnode*,bnode*);
int bnode_destroy(bnode* self);

#endif
