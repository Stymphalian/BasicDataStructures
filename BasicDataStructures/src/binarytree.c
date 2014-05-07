#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"

#include "binarytree.h"
#include "bnode.h"
#include "stack.h"
#include "queue.h"


#ifndef DEBUG
#define DEBUG 0
#endif

typedef struct heightStack_t hstk;
struct heightStack_t{
	bnode* n;
	int left;
	int right;
};

static int _my_binarytree_swapNodes(bnode* b1, bnode* b2);
static int _my_binarytree_removeNode_noChild(bnode* node);
static int _my_binarytree_removeNode_oneChild(bnode* node);
static int _my_binarytree_removeNode_twoChild(bnode* node);
static int _my_binarytree_removeNode(binarytree* bt, bnode* key);

static void _my_binarytree_height_modParent(hstk* current, Stack* stack);
static int _my_binarytree_traverse(bnode* r,int type,void* it,int len);



static int binarytree_height(binarytree* tree);
static void* binarytree_traverse(binarytree* tree, int* len, int type);
static int binarytree_insert(binarytree* tree, void* key, void* val);
static int binarytree_remove(binarytree* tree, void* key);
static void* binarytree_find(binarytree* tree, void* key);
static void* binarytree_get(binarytree* tree, void* key);

binarytree* binarytree_create(bnode* root,binarytree* left,binarytree* right,
				int keySize, int valSize,
				binarytree_keyCmp keyCmp,
				binarytree_itemDestroy iDestroy)
{
	if(DEBUG){printf("creating binary tree\n");}
	binarytree* tree = malloc(sizeof(binarytree));
	if(tree == NULL) {return NULL;}

	tree->root = root;
	tree->size = 0;
	if(root != NULL){
		tree->size++; /* counting root */
		if(left != NULL){
			tree->root->left = left->root;
			tree->size += left->size; /* counting left*/
		}

		if(right != NULL){
			tree->root->right = right->root;
			tree->size += right->size; /* counting right*/
		}
	}
	tree->keySize = keySize;
	tree->valSize = valSize;
	tree->keyCmp = keyCmp;
	tree->itemDestroy = iDestroy;
	
	tree->height = &binarytree_height;
	tree->traverse = &binarytree_traverse;
	tree->insert = &binarytree_insert;
	tree->remove = &binarytree_remove;
	tree->find = &binarytree_find;
	tree->get = &binarytree_get;
	
	return tree;
}

int binarytree_destroy(binarytree* tree){
	if(DEBUG){printf("destroying binary tree\n");}
	if(tree ==NULL ){return 0;}
	if(tree->root == NULL) {
		if(DEBUG){printf("binarytree.c destroy: empty tree\n");}
		return 0;
	}

	int pos = 0;
	int maxSize =(tree->size/2) + 1;
	int count = 0;
	bnode** stack =NULL;
	bnode** top = NULL;
	bnode* current= NULL;

	stack = malloc(sizeof(bnode*)*maxSize);
	top = stack;

	*top = tree->root;
	top++;
	pos++;

	do{
		top--;
		pos--;
		current = *top; 	
		if(current == NULL) { continue;}

		if( pos >= maxSize-2){
			stack = realloc(stack,sizeof(bnode*)*maxSize*2);
			top = (stack + pos );
			maxSize = 2*maxSize;
		}

		if( current->left != NULL){
			*top = current->left;
			top++;
			pos++;
		}

		if( current->right != NULL){
			*top = current->right;
			top++;
			pos++;
		}


		if(!tree->itemDestroy){
			tree->itemDestroy(current->key, current->value);
		}
		if(DEBUG){printf("binarytree.c destroy freeing %p \n", current);};
		bnode_destroy(current);
		count++;
	}while(pos > 0);
	free(stack);

	if( count != tree->size){
		if(DEBUG){printf("not all nodes were freed\n");}
	}

	memset(tree,0,sizeof(binarytree));
	free(tree);
	return 1;
}


 
static int binarytree_insert(binarytree* tree, void* val, void* key){
	if(tree == NULL) {return 0;}
	
	int directionFlag = 1;
	int rs;
	bnode* prev;
	bnode* current;
	bnode* newnode;
   
	newnode = bnode_create(key,val,tree->keySize,tree->valSize,0,0,0);
	if(newnode== NULL){ return 0;}
	
	current = tree->root;
	prev = current;

	if(current == NULL){
		tree->root = newnode;
		tree->size= 1;
		if(DEBUG){printf("%d insert : %p\n",newnode->value, newnode);}
		return 1;
	}else{
		while(current != NULL){
			prev = current;
			rs = tree->keyCmp(key,current->key);
			if(rs == 0){ /* equal */
				newnode->right = current->right;
				if(current->right != NULL){
					current->right->parent = newnode;
				}
				current->right = newnode;
				newnode->parent = current;
				tree->size++;
				if(DEBUG){printf("%d insert : %p\n", newnode->value,newnode);}
				return 1;
			}else if(rs == 1){ /*right is bigger key < current->key*/
				current = current->left;
				directionFlag = 0;
			}else{ /* left is bigger , key > current->key */
				current = current->right;
				directionFlag = 1;
			}
		}

		newnode->parent=prev;
		if(directionFlag){
			prev->right = newnode;
		}else{
			prev->left = newnode;
		}
		tree->size++;
	}

	if(DEBUG){printf("%d insert : %p\n",newnode->value, newnode);}
	return 1;
}

static void* binarytree_find(binarytree* tree,void* key){
	if( tree == NULL) { return NULL;}

	int rs = 0;
	bnode* current = tree->root;

	while( current != NULL){
		rs = tree->keyCmp(key,current->key);
		if( rs ==0 ){ /* equal */
			return current->value;
		}else if( rs == 1){ /* right is bigger  key < current->key */
			current = current->left;
		}else{ /* left is bigger, key > current->key */
			current = current->right;
		}
	}
	return NULL;
}

static void* binarytree_get(binarytree* tree,void* key){
	if( tree == NULL) { return NULL;}

	int rs = 0;
	bnode* current = tree->root;

	while( current != NULL){
		rs = tree->keyCmp(key,current->key);
		if( rs ==0 ){ /* equal */
			void* copy = malloc(tree->valSize);
			if(!copy){return NULL;}
			memcpy(copy, current->value, tree->valSize);
			return copy;
		}else if( rs == 1){ /* right is bigger  key < current->key */
			current = current->left;
		}else{ /* left is bigger, key > current->key */
			current = current->right;
		}
	}
	return NULL;
}





int binarytree_swapNodes(bnode* b1, bnode* b2){
	return _my_binarytree_swapNodes(b1,b2);
}
static int _my_binarytree_swapNodes(bnode* b1,bnode* b2){
	if( b1 == NULL  || b2 == NULL) {return 0;}
	if( b1 == b2){return 1;}

	int flag=0;
	if( b1->parent == b2){
		bnode* temp = b2;
		b2 = b1;
		b1 = temp;
		flag =1;
		if(DEBUG){printf("swapped and flagged \n");}
	}else if( b2->parent == b1){
		flag =1;
		if(DEBUG){printf("not swapped, but flagged\n");}
	}

	if(DEBUG){
		printf(" b1, b2\n");
		printf("b1 = %p\n", b1);
		printf("b2 = %p\n", b2);
	}
/* b1 is always atleast an ancestor of b2 */

	bnode* parent1 = b1->parent;
	bnode* left1 = b1->left;
	bnode* right1 = b1->right;

	bnode* parent2 = b2->parent;
	bnode* left2 = b2->left;
	bnode* right2 = b2->right;

	if( parent1 != NULL){
		if(parent1->right == b1){parent1->right = b2;}
		else if ( parent1->left == b1){parent1->left = b2;}
	}

	if( parent2 != NULL){
		if(parent2->right == b2){parent2->right = b1;}
		else if (parent2->left == b2){parent2->left = b1;}
	}

	if( left1 != NULL){ left1->parent = b2;}
	if( left2 != NULL){ left2->parent = b1;}
	if( right1 != NULL){ right1->parent = b2;}
	if( right2 != NULL){ right2->parent = b1;}
	
	b1->parent = parent2;
	b1->left = left2;
	b1->right = right2;
	b2->parent = parent1;
	b2->left = left1;
	b2->right = right1;

	if( flag ){
		b1->parent = b2;
		if(right1 == b2 ){
			b2->right= b1;
		}else if( left1 == b2){
			b2->left = b1;
		}
	}

	if(DEBUG){
		printf(" b1 then b2: parent, left, right\n");
		printf("%p\n",b1->parent);
		printf("%p\n",b1->left);
		printf("%p\n",b1->right);
		printf("%p\n",b2->parent);
		printf("%p\n",b2->left);
		printf("%p\n",b2->right);
	}
	return 1;	
}

static int _my_binarytree_removeNode_oneChild(bnode* node){
	if(node == NULL) { return 0;}
	if(node->parent == NULL) {return 0;}
	bnode* parent = node->parent;

	/* which child (left or right?) is the one child*/
	if(node->right != NULL){
		/* let the parent inherit the one child of node*/
		if(parent->right == node){
			parent->right = node->right;
		}else{
			parent->left = node->right;
		}	
		/* the child of node points to node's parent */
		node->right->parent = parent;
	}else{
		/* let the parent inherit the one child of node*/
		if(parent->right == node){
			parent->right = node->left;
		}else{
			parent->left = node->left;
		}
		/* the child of node points to node's parent */
		node->left->parent = parent;
	}
	return 1;
}
static int _my_binarytree_removeNode_twoChild(bnode* node){
	if( node== NULL) {return 0;}
	if( node->right == NULL || node->left == NULL) {return 0;}

	bnode* current;	
	bnode* prev;
	/* find the next node of the sequence */
	prev = node;
	current= node->right;
	while( current != NULL){
		prev = current;
		current = current->left;
	}
	/* exchange node and the candidate */
	int rs =0;
	rs =_my_binarytree_swapNodes(prev, node);


	/* perform a single, or no_child removal */	
	if( (node->left == NULL && node->right == NULL) && rs ==1){
		return _my_binarytree_removeNode_noChild(node);
	}else{
		return _my_binarytree_removeNode_oneChild(node);
	}
}


static int _my_binarytree_removeNode_noChild(bnode* node){
	if(node == NULL ) { return 0;}
	if( node->left  != NULL || node->right != NULL){return 0;}
	if( node->parent != NULL){
		if( node->parent->right == node){
			node->parent->right = NULL;
		}else{
			node->parent->left = NULL;
		}
	}
	return 1;
}

static int _my_binarytree_removeNode(binarytree* tree, bnode* node){
	if (!tree  || !node ) {return 0;}
	int rs =0;	

	/* a tempparent such that "node" will not have a null parent */
	bnode tempParent = {0,0,0,0,0,0,0};
	if( node->parent == NULL  || (node == tree->root)){
		bnode_init(&tempParent,0,0,0,0,NULL,NULL,node); /* always right child*/
		node->parent = &tempParent;
	}

	/* no children */
	if(node->left == NULL  && node->right == NULL){
		rs= _my_binarytree_removeNode_noChild(node);
	/* two children */
	}else if( node->left != NULL && node->right != NULL){
		if(DEBUG) {printf("removing two children %p\n", node);}
		rs = _my_binarytree_removeNode_twoChild(node);
	/* one child */
	}else{
		rs =_my_binarytree_removeNode_oneChild(node);
	}


	/* handle the root case*/
	if( node == tree->root && rs){
		tree->root = tempParent.right;
	}

	/* if tempParent was used, undo the pointers */
	if( tempParent.right != NULL){
		tempParent.right->parent = NULL;
		tempParent.right = NULL;
	}

	/* delete the node and decrement the size */
	if(tree->itemDestroy){tree->itemDestroy(node->key,node->value);}
	if( DEBUG) { printf(" binarytree.c removeNode %p freed\n",node);}
	bnode_destroy(node);
	tree->size--;
	return rs;
}


static int binarytree_remove(binarytree* tree, void* key){
	if ( tree == NULL) {return 0;}

	int rs =0;
	bnode* current = tree->root;

	while( current != NULL ){
		rs = tree->keyCmp(key, current->key);
		if( rs == 0){ /* == */
			if( DEBUG){
				printf(" binarytree_remove, &node %p\n",current);
			}
			if(_my_binarytree_removeNode(tree,current)){
				return 1;
			}else{
				break;
			}
		}else if(rs == 1){ /* key < current->key */
			current = current->left;
		}else{ /* key > current->key */
			current = current->right;
		}
	}
	return 0;
}

static void _my_binarytree_height_modParent(hstk* current, Stack* stack){
	if( current == NULL  || stack == NULL){return;}

	bnode* parent = NULL;
	hstk*  temp = NULL;
	hstk item = {0,0,0};
	int max=(current->left >= current->right) ? current->left : current->right;
	if( current->n != NULL){ parent = current->n->parent;}
	if( parent == NULL) {return;}

	temp = stack->pop(stack);
	if( temp == NULL){return;}
	item = *temp;

	if( item.n == parent){
		if(parent->left == current->n){item.left = max+1;}
		else{item.right = max+1;}	
		stack->push(stack,&item);

	}else{

		temp = stack->pop(stack);
		if( temp == NULL){
			stack->push(stack,&item);
			return;
		}

		hstk item2 = *temp;
		if(parent->left == current->n){item2.left = max+1;}
		else{item2.right = max+1;}
		stack->push(stack,&item2);
		stack->push(stack,&item);
	}
}

static int binarytree_height(binarytree* bt){
	if( bt == NULL) {return -1;}
	Stack* s = Stack_create( bt->size, sizeof(hstk));
	int height = 0;
	hstk item = {bt->root,-1,-1} ;
	hstk tempItem= {0,0,0};
	hstk* current;

	s->push(s,&item);
	while( !(s->isEmpty(s)) ){
		current= (hstk*) s->pop(s);
		if( current == NULL ){continue;}
		if( current->n == NULL) {continue;}
		item = *current; /* this should copy what is in current into item*/	

		
		/* if the left/right child is null, mark as visited */
		if( item.n->left == NULL){
			item.left = 0;
		}
		if(	item.n->right == NULL){
			item.right = 0;
		}

		/* if we have both the heights of our left & right children, 
		 * modify the height of our parent with the max(left, right) +1
		 */
		if(item.left != -1 && item.right !=-1){
			/* if this node is root, reutrn the final height */
			if( item.n == bt->root ){
				height =(item.left >= item.right) ? item.left+1 : item.right +1;
				break;
			}
			_my_binarytree_height_modParent(&item,s);
			continue;
		}


		s->push(s, &item);
		if( item.left == -1 && item.n->left != NULL){
			tempItem.n = item.n->left;
			tempItem.left  = -1;
			tempItem.right = -1;
			s->push(s,&tempItem);
		}

		if( item.right == -1 && item.n->right != NULL){
			tempItem.n = item.n->right;
			tempItem.left = -1;
			tempItem.right= -1;
			s->push(s,&tempItem);
		}

	}

	Stack_destroy(s);
	return height;
}



static int _my_binarytree_traverse(bnode* root,int type,
				void* it,int len){
	if( root == NULL) {return -1;}
	int pos = 0;
	int count =0;
	bnode* current = NULL;
	void* temp = 0;


	switch(type){
		case(1):{	/* left */			
				Stack* s = Stack_create(len, sizeof(bnode*));
				if( s== NULL ) {return -1;}
				s->push(s,&root);
				pos = 0;

				while(!s->isEmpty(s)){
					temp  = s->pop(s);	
					if( temp == NULL ){continue;}
					current = *((bnode**) temp);
					if( current == NULL) {continue;}

					if( pos == len) {break;}
					memcpy(it+pos*current->valSize,
									current->value,current->valSize);
					count++;
					pos++;

					if(current->right != NULL){
						s->push(s, &current->right);
					}
					if(current->left != NULL){
						s->push(s,&current->left);
					}
				}
				Stack_destroy(s);
				}break;

		case(2):{/* middle */
				Stack* s = Stack_create(len, sizeof(bnode*));
				Stack* s2 = Stack_create(len, sizeof(int) );
				int num = 0;
				pos = 0;
				current = NULL;
				s->push(s,&root);
				s->push(s2, &num);
				void* temp=0;
				void* temp2=0;

				while( !s->isEmpty(s) && !s2->isEmpty(s2)){
					temp = s->pop(s);
					temp2 = s2->pop(s2);
					if(!temp || !temp2){ continue;}
					current = *((bnode**)temp);
					num = *((int*)temp2);		
					if( current == NULL ){ continue;}

					if( num != 0 ||
					  (current->left == NULL && current->right == NULL))
					{
						if( pos == len ) { break; };
						memcpy(it+pos*current->valSize,
									current->value,current->valSize);
						count++;
						pos++;	
						continue;
					}


					if( current->right != NULL){
						num = 0;
						s->push(s, &current->right);
						s2->push(s2,&num);
					}

					num = 1;
					s->push(s,&current);
					s2->push(s2,&num);
				
					if( current->left != NULL){
						num = 0;
						s->push(s,&current->left);
						s2->push(s2,&num);
					}
				};

				Stack_destroy(s);
				Stack_destroy(s2);

				}break;
		case(3):{ /* right */
				Stack* s = Stack_create(len, sizeof(bnode*));
				Stack* s2 = Stack_create(len, sizeof(int));
				int num = 0;
				int target = 0;
				current = 0;
				pos = 0;
				s->push(s,&root);
				s2->push(s2,&num);
				void* temp=0;
				void* temp2=0;


				while( !s->isEmpty(s) && !s2->isEmpty(s2)){
					temp = s->pop(s);
					temp2 = s2->pop(s2);

					if( !temp || !temp2){continue;}
					current = *((bnode**)temp);
					num = *((int*)temp2);
					if( current == NULL ){ continue;}

					if( current->left != NULL && current->right != NULL){
						target = 2;
					}else if (current->left== NULL && current->right== NULL ){
						target = 0;
					}else{
						target = 1;
					}

					if(num == target){
						if(pos == len){ break;}
						memcpy(it+pos*current->valSize,
									current->value,current->valSize);
						count++;
						pos++;
						continue;			
					}

					target = num;
					num++;
					s->push(s,&current);
					s2->push(s2,&num);

					if( current->right != NULL && target == 0){
						num = 0;
						s->push(s,&current->right);
						s2->push(s2,&num);
					}

					if( current->left != NULL && target == 0){
						num = 0;
						s->push(s, &current->left);
						s2->push(s2, &num);
					}
				}

				Stack_destroy(s);
				Stack_destroy(s2);

				}break;
		case(4):{/* level */
				Queue* q = Queue_create(len, sizeof(bnode*));	
				q->enqueue(q,&root);
				pos = 0;
				current = NULL;
				void* temp=0;

				while(!q->isEmpty(q)){
					temp =q->dequeue(q);
					if( temp == NULL) {continue;}
					current = *((bnode**)temp);
					if( current == NULL ){ continue;}

					if(pos == len){ break;}
					memcpy(it+pos*current->valSize,
									current->value,current->valSize);
					count++;
					pos++;

					if( current->left != NULL){
						q->enqueue(q,&current->left);
					}

					if(current->right != NULL){
						q->enqueue(q,&current->right);
					}
				}
				Queue_destroy(q);
				}break;
	}

	return count;
}

static void* binarytree_traverse(binarytree* tree, int* len, int type){
	if( tree == NULL) {return NULL;}
	void* iterator = malloc( tree->valSize * tree->size);
	if( iterator == NULL) {return NULL;}

	*len =_my_binarytree_traverse(tree->root,type,iterator,tree->size);
	return iterator;
}
