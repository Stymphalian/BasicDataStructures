#ifndef AVLTREE_H
#define AVLTREE_H

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <list>
#include "avlnode.h"

template <class K, class T>
class avltree {
	avlnode<K,T>* root;
	unsigned size;

	private:
		bool isExternal(avlnode<K,T>* node);
		bool isInternal(avlnode<K,T>* node);
		unsigned _height(avlnode<K,T>* node);
		avlnode<K,T>* restructure(avlnode<K,T>* x,avlnode<K,T>* y,avlnode<K,T>* z);
		avlnode<K,T>* bst_insert(K key, T element);
		avlnode<K,T>* removeNoChildren(avlnode<K,T>* current);
		avlnode<K,T>* removeOneChildren(avlnode<K,T>* current);
		avlnode<K,T>* removeTwoChildren(avlnode<K,T>* current);
		avlnode<K,T>* bst_remove(K key);
		avlnode<K,T>* findNode(avlnode<K,T>* node, K key);
		avlnode<K,T>* findInOrderSuccesor(avlnode<K,T>* node);
		avlnode<K,T>* findUnbalancedNode(avlnode<K,T>* node);
		void swapNodes(avlnode<K,T>* n1, avlnode<K,T>* n2);
		void _traverse(avlnode<K,T>* node, std::list<T>* list, int type);
		void _formatted_print(avlnode<K,T>* node);
	public:
		avltree();
		~avltree();

		int insert(K key, T elem);
		T remove(K key);
		T find(K key);
		std::list<T>* traverse(int type);
		unsigned getSize() { return size;}
		unsigned height();
		bool isEmpty() {return size == 0;}
		void formatted_print();
};

template<class K, class T>
avltree<K,T>::avltree(){
	root = 0;
	size = 0;
}

template<class K, class T>
avltree<K,T>::~avltree(){
	if( root == 0 ){return;}

	std::list<avlnode<K,T>*> stack;
	avlnode<K,T>* current = 0;
	unsigned original_count = size;

	stack.push_back(root);
	while(!stack.empty()){
		current = stack.front();

		if( current->left != 0 ){stack.push_back(current->left);}
		if( current->right != 0 ){stack.push_back(current->right);}
		delete current;
		stack.pop_front();
		--size;
	}	

	root = 0;
	printf("%d out of %d were deleted\n", original_count - size, original_count);
}

template<class K,class T>
int avltree<K,T>::insert(K key, T elem){
	avlnode<K,T>* added = bst_insert(key,elem);

	// Balance the tree.
	avlnode<K,T>* unbalanced_parent = findUnbalancedNode(added);

	// no unbalanced nodes were found return sucess.
	if( unbalanced_parent == 0 ){ return 1;}

	// we note the >= defaults to right.
	// this is because of how we handle duplicate keys
	// we add them as the right child of the newnode.
	avlnode<K,T>* z = unbalanced_parent;
	avlnode<K,T>* y = (added->key >= z->key) ? z->right : z->left; 
	avlnode<K,T>* x = (added->key >= y->key) ? y->right : y->left;

	avlnode<K,T>* subtree = restructure(x,y,z);
	if( subtree ==0 ){return 0;}

	// repair for case in whcih the unbalacned node was root.
	if( unbalanced_parent == root ){ root = subtree; root->parent = 0;}	
	return 1;	
}


/*
 *   z(a)    <-- first unbalanced node  -->  z(a)
 *  / \                                     /   \
 * t1   y(b)									    t1   y(c)
 *    / \												  /    \
 *   t2	x(c)										    x(b)   t4
 *		  / \											   / \
 *     t3   t4										  t2 t3			
 *
 *
 * x,y,z are the nodes which we will restructure, which we relael as a, b, c
 * t1, t2, t3, t4 are the subtrees for each of those nodes(z,y,x).
 * These cover the two types of restructures, but we will have to 
 * handle each of their syymetries.
 */
template<class K,class T>
avlnode<K,T>* avltree<K,T>::restructure(avlnode<K,T>* x, avlnode<K,T>* y, avlnode<K,T>* z){
	if( !x || !y || !z ) {return 0;}
	avlnode<K,T>* a = 0;
	avlnode<K,T>* b = 0;
	avlnode<K,T>* c = 0;
	avlnode<K,T>* t1 = 0;
	avlnode<K,T>* t2 = 0;
	avlnode<K,T>* t3 = 0;
	avlnode<K,T>* t4 = 0;

	// this big if..else is used in order to determine the type 
	// of the restructure and to setup the appropriate x,y, z nodes
	// and the a,b,c,d subtrees for those nodes.
	if( z->right == y  && y->right == x ){
		a = z;
		b = y;
		c = x;
		t1 = z->left;
		t2 = y->left;
		t3 = x->left;
		t4 = x->right;
	}else if( z->left == y && y->left == x) {
		a = x;
		b = y;
		c = z;
		t1 = x->left;
		t2 = x->right;
		t3 = y->right;
		t4 = z->right;
	}else if( z->right == y && y->left == x) {
		a = z;
		b = x;
		c = y;
		t1 = z->left;
		t2 = x->left;
		t3 = x->right;
		t4 = y->right;
	}else if( z->left == y && y->right == x ){
		a = y;
		b = x;
		c = z;
		t1 = y->left;
		t2 = x->left;
		t3 = x->right;
		t4 = z->right;
	}else {
		printf("Error: %d\n", __LINE__);
		fflush(stdout);
		return 0;
	}

#if 0
	#define foo(a) ((a!=0) ? a->key: -1 )
	printf("a=%d, b=%d, c=%d\n",(int)a->key,(int)b->key,(int)c->key);
	printf("t1=%d,t2=%d,t3=%d,t4=%d\n", foo(t1), foo(t2), foo(t3), foo(t4));
	#undef foo
#endif

	avlnode<K,T>* zParent = z->parent;
	a->init(b,t1,t2,a->key, a->element);
	b->init(zParent,a,c,b->key,b->element);
	c->init(b,t3,t4,c->key,c->element);

	//set the parents.
	if(t1) { t1->parent = a; }
	if(t2) { t2->parent = a; }
	if(t3) { t3->parent = c;}
	if(t4) { t4->parent = c;}

	//repair zParent's child pointer
	if( zParent != 0 && zParent->left == z ){
		zParent->left = b;
	}else if( zParent!= 0){ // !zParent && zParent->right == z 
		zParent->right = b;
	}

	return b;
}

template<class K,class T>
T avltree<K,T>::remove(K key){
	avlnode<K,T>* node =  bst_remove(key);
	if( node == 0 ){return 0;}

	// save the element and delete the node.
	avlnode<K,T>* parent = node->parent;
	T e = node->element;
	delete node;
	--size;

	//balance the tree
	avlnode<K,T>* unbalanced_node = 0;
	avlnode<K,T>* z = 0;
	avlnode<K,T>* y = 0;
	avlnode<K,T>* x = 0;
	avlnode<K,T>* last = (parent == 0) ? root : parent;
	avlnode<K,T>* subtree = 0;

	while( (unbalanced_node = findUnbalancedNode(last)) != 0 ){
		z = unbalanced_node;
		y = ( _height(z->left) > _height(z->right) )? z->left : z->right;
		x = ( _height(y->left) > _height(y->right) )? y->left : y->right;
		subtree = restructure(x,y,z);
		if( subtree == 0 ) {continue;}
	
		// repair the root pointer	
		if( root ==  unbalanced_node ){
			root = subtree;
			root->parent = 0;
		}
	
		last = subtree;	
	}
	return e;
}

template<class K,class T>
T avltree<K,T>::find(K key){
	avlnode<K,T>* current = findNode(root,key);
	if( current == 0 ){return 0;}
	if( current->key != key ) { return 0;}
	return current->element;
}

template<class K,class T>
bool avltree<K,T>::isExternal(avlnode<K,T>* node){
	if (node == 0) {return false;}
	return (node->left == 0 && node->right == 0);
}

template<class K,class T>
bool avltree<K,T>::isInternal(avlnode<K,T>* node){
	return !isExternal(node);
}

template<class K,class T>
unsigned avltree<K,T>::_height(avlnode<K,T>* node){
	if( node == 0 ) {return 0;}
	unsigned left = _height(node->left);
	unsigned right = _height(node->right);
	unsigned max = (left < right ) ? right : left;	
	return max + 1;
}

template<class K, class T>
unsigned avltree<K,T>::height(){
	return _height(root);
}

template<class K,class T>
avlnode<K,T>* avltree<K,T>::findNode(avlnode<K,T>* node, K key){
	if( node == 0){return 0;}
	avlnode<K,T>* current = node;
	while( current != 0){
		avlnode<K,T>* left = current->left;
		avlnode<K,T>* right = current->right;
		if( key == current->key){
			return current;
		}else if( left != 0  && key <= current->key ){
			current = left;
		}else if( right != 0 && key >= current->key){
			current = right;
		}else{
			return 0;
		}
	}
	return 0;
}

template<class K,class T>
avlnode<K,T>* avltree<K,T>::bst_insert(K key, T elem){
	if( size == 0  || root == 0){
		root = new avlnode<K,T>(0,0,0,key,elem);
		size = 1;
		return root;
	}

	avlnode<K,T>* current = root;
	while(current != 0 ){
		avlnode<K,T>* left = current->left;
		avlnode<K,T>* right = current->right;
		if( key == current->key ){
			// If we have a duplicate key.
			// assume the original node is x and the newnode is y.
			// we make y the parent of x,and x as the right child of y.
			avlnode<K,T>* parent = current->parent;
			avlnode<K,T>* newnode = new avlnode<K,T>(parent,0,current,key,elem);	
		
			current->parent = newnode;

			if( parent != 0 && isleftchild(parent,current) ){
				parent->left = newnode;
			}else if( parent != 0 && isrightchild(parent,current)) {
				parent->right = newnode;
			}
			++size;
			return newnode;	
		} else if( key < current->key ){
			if( left == 0 ){
				current->left = new avlnode<K,T>(current,0,0,key,elem);
				++size;
				return current->left;
			}else{
				current = left;
			}
		}else {
			if( right == 0 ){
				current->right = new avlnode<K,T>(current,0,0,key,elem);
				++size;
				return current->right;
			}else{
				current = right;
			}
		}

	}

	return 0;
}

template<class K,class T>
bool isleftchild(avlnode<K,T>* parent, avlnode<K,T>* current){
	if( parent == 0  || current ==0){ return false;}
	return (parent->left == current);
}
template<class K,class T>
bool isrightchild(avlnode<K,T>* parent, avlnode<K,T>* current){
	if(parent == 0 || current == 0 ){return false;}
	return (parent->right == current);
}


template<class K,class T>
avlnode<K,T>* avltree<K,T>::bst_remove(K key){
	avlnode<K,T>* current = findNode(root,key);
	if( current == 0 ){return 0;}
	if( current->key != key ){return 0;}


	/* This ensures that a parent always exist, even if
	 * it is only just a dummy */
	int parent_flag = 0;
	avlnode<K,T> dummy_parent;
	if(current->parent == 0 ){
		current->parent = &dummy_parent;
		dummy_parent.left = current;
		dummy_parent.key = -2;
		parent_flag = 1;
	}

	if( isExternal(current) ){
		current = removeNoChildren(current);
	}else if( current->left == 0 && current->right){
		current = removeOneChildren(current);
	}else if( current->left && current->right == 0 ){
		current = removeOneChildren(current);
	}else{
		current = removeTwoChildren(current);
	}


	if( parent_flag ){
		current->parent = 0;
		if( dummy_parent.left ) {dummy_parent.left->parent = 0;}
		if( dummy_parent.right ) {dummy_parent.right->parent = 0;}
		parent_flag = 0;
	}

	return current;
}

template<class K, class T>
avlnode<K,T>*  avltree<K,T>::removeNoChildren(avlnode<K,T>* current){
	if(!current) {return 0;}
	if( current->left || current->right){return 0;}

	if( isleftchild( current->parent, current)){
		current->parent->left = 0;
	}else if( isrightchild(current->parent,current) ){
		current->parent->right = 0;
	}

	if (current == root) {root = 0;}
	return current;
}

template<class K, class T>
avlnode<K,T>*  avltree<K,T>::removeOneChildren(avlnode<K,T>* current){
	if(!current) {return 0;}
	if(current->left && current->right ) {return 0;}
	if( current->left == 0 && current->right == 0) {return 0;}
	avlnode<K,T>* child = (current->left) ? current->left : current->right;
	avlnode<K,T>* parent = current->parent;

	if( isleftchild(current->parent, current) ){
		parent->left = child;
	} else if ( isrightchild(current->parent,current) ) {
		parent->right = child;
	}

	child->parent = parent;

	if( current == root ){ root = child; }
	return current;
}

template<class K, class T>
avlnode<K,T>* avltree<K,T>::removeTwoChildren(avlnode<K,T>* current){
	if(!current) {return 0;}
	if( current->left && current->right ){
		avlnode<K,T>* inorder = findInOrderSuccesor(current);
		if(inorder == 0 ){ return 0;}
		swapNodes(inorder, current);
	
		if( inorder->right ){
			removeOneChildren(inorder);
		}else{
			removeNoChildren(inorder);
		}	

		return inorder;
	}
	return 0;
}


template<class K,class T>
avlnode<K,T>* avltree<K,T>::findInOrderSuccesor(avlnode<K,T>* node){
	if(isExternal(node)) {return 0;}
	if(node->right == 0 ){return 0;}
	avlnode<K,T>* current = node->right;
	while( current->left != 0 ){
		current = current->left;
	}
	return current;
}


template<class K,class T>
avlnode<K,T>* avltree<K,T>::findUnbalancedNode(avlnode<K,T>* node){
	if( node == 0 ) { return 0;}
	avlnode<K,T>* current = node;
	unsigned left = 0;
	unsigned right = 0;
	do {
		left = _height(current->left);
		right = _height(current->right);
		if( abs(left - right) >=2 ){break;}
		current = current->parent;
	
	}while( current != 0 );
	return current;
}

template<class K,class T>
void avltree<K,T>::swapNodes(avlnode<K,T>* n1, avlnode<K,T>* n2){
	K temp_key = n1->key;
	T temp_element= n1->element;

	n1->key = n2->key;
	n1->element = n2->element;

	n2->key = temp_key;
	n2->element = temp_element;
}

template<class K,class T>
std::list<T>* avltree<K,T>::traverse(int type){
	std::list<T>* values = new std::list<T>;
	_traverse(root,values,type);
	return values;
}

template<class K,class T>
void avltree<K,T>::_traverse(avlnode<K,T>* node, std::list<T>* list, int type){
	if( !node ){return ;}
	switch(type){
		case(0):{
			list->push_back(node->element);
			if( node->left != 0 ) {_traverse(node->left,list,type); }
			if( node->right != 0) {_traverse(node->right,list,type);} 	
			break;
		}
		case(1):{
			if( node->left != 0 ) {_traverse(node->left,list,type); }
			list->push_back(node->element);
			if( node->right != 0) {_traverse(node->right,list,type);} 
			break;
		}
		case(2):{
			if( node->left != 0 ) {_traverse(node->left,list,type); }
			if( node->right != 0) {_traverse(node->right,list,type);} 
			list->push_back(node->element);
			break;
		}
		case(3):{
			std::list<avlnode<K,T>*> stack;
			avlnode<K,T>* current = 0;
 			stack.push_back(node);
			while(!stack.empty() ){
				current = stack.front();
				stack.pop_front();
				list->push_back(current->element);	
				if(current->left != 0 ){stack.push_back(current->left);}
				if(current->right != 0 ){stack.push_back(current->right);}
			}				  
			break;
		}
	}
}
template <class K,class T>
void avltree<K,T>::formatted_print(){
	_formatted_print(root);
}


template<class K, class T>
void avltree<K,T>::_formatted_print(avlnode<K,T>* node){
	/* get the element */
	std::list<K> elem_stack;
	std::list<int> valid_stack;
	std::list<avlnode<K,T>*> stack;
	avlnode<K,T>* current = 0;
	stack.push_back(node);

	while(!stack.empty()){
		current = stack.front();
		stack.pop_front();

		if( current == 0 ){
			elem_stack.push_back(0);
			valid_stack.push_back(0);
			continue;
		}else{
			elem_stack.push_back(current->key);
			valid_stack.push_back(1);
		}

		if( current->left != 0 ){ stack.push_back( current->left); }
		else { stack.push_back(0);}
		if( current->right != 0 ){ stack.push_back( current->right);}
		else { stack.push_back(0);}
	}

	int element_len = 6; // this shoudl be dynamically found
	int h = _height(node);
	int total_num_nodes = (int) pow(2,h) -1;
	int last_row_nodes = (int) pow(2,h-1);
	int row_len = last_row_nodes*(element_len + 1);
	char num_str[element_len +1];

	int target = 1;
	int count = 0;
	while( !elem_stack.empty() ){
		// zero out the num_str
		memset(num_str,0,sizeof(char)*element_len);

		// get the element
		int num = (int) elem_stack.front();
		int valid_flag = valid_stack.front();
		elem_stack.pop_front();
		valid_stack.pop_front();


		// find the length of the element... if there is one.
		int len = 0;	
		if( valid_flag == 0 ){
			len = 0; 
		}else{
			sprintf(num_str,"%d",num);
			len = strlen(num_str);
		}

		// print out the spaces and the number
		
		// print the left-side of the spaces
		int i = 0;
		int limit = row_len/2;
		int other = 0;
		if( limit + limit != row_len){ other++;}
		if( len/2 + len/2 < len ) {other--;}
		limit -= len/2;
		for( i = 0; i < limit; ++i ){
			printf(" ");
		}

		// print the element
		if( valid_flag ){ printf("%d",num);}

		// print the right side of the spaces
		for( i=0;i < limit+other; ++i){
			printf(" ");
		}

		//increment our count
		//This is used to keep track of which level we are on
		//in our tree
		++count;
		if( count >= (int) pow(2,target)-1 ){
			row_len = row_len/2;
			target++;
			printf("\n");
		}
		if(count >= total_num_nodes ){break;}
	}
	printf("\n");
	return;
}


#endif
