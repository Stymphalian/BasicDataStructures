#include <stdio.h>
#include <stdlib.h>

typedef struct PQueue_elem_t PQueue_elem_t;
struct PQueue_elem_t{
	void* data;
	int key;
};
typedef struct PQueue_t PQueue_t;
struct PQueue_t{
	PQueue_elem_t* array;
	int capacity;
	int size;
};

PQueue_t* create_PQueue_t(int size);
void destroy_PQueue_t(PQueue_t* self);
int PQueue_insert(PQueue_t* self,void* data, int key);
void* PQueue_remove(PQueue_t* self);
void* PQueue_peek(PQueue_t* self);
int PQueue_size(PQueue_t* self);


PQueue_t* create_PQueue_t(int size){
	if(size < 2){size = 2;}

	PQueue_t* pq = malloc(sizeof(PQueue_t));
	pq->array = malloc(sizeof(PQueue_elem_t)*size);
	pq->size = 1; // start from 1
	pq->capacity = size;
	
	return pq;
}

void destroy_PQueue_t(PQueue_t* self){
	if(!self){return;}
	free(self->array);
	free(self);
}

// 2*pos == left child
// 2*pos + 1 == right child
// pos/2 == parent
int PQueue_insert(PQueue_t* self,void* data, int key){
	if(self->size >= self->capacity){return 0; }
	
	int index = self->size;
	int parent_index = index/2;
	self->size++;
	
	// bubble up element to the correct position
	while( parent_index != 0 && key < self->array[parent_index].key){
		// swap the nodes
		self->array[index].data = self->array[parent_index].data;
		self->array[index].key = self->array[parent_index].key;

		index = parent_index;  // we have swapped places with parent
		parent_index = index/2; // get the new parent index;
	}
	
	self->array[index].data = data;
	self->array[index].key = key;
	return 1;
}

void* PQueue_remove(PQueue_t* self){
	if( self->size == 1 ){return 0;} // no elements to remove
	void* return_data = self->array[1].data;

	// remove the node, therefore decrease the size
	self->size--;
	// put the last nodes data into the top node
	self->array[1].data = self->array[self->size].data;
	self->array[1].key = self->array[self->size].key;
	
	//bubble down to the correct position
	int index = 1;
	int left_child = index*2;
	int right_child = index*2+1;
	int least_child = left_child;
	while(index < self->size){

		// determine which child is the 'least'
		if( left_child >= self->size){break;}
		if( right_child < self->size){
			least_child = left_child;
			if( self->array[left_child].key > self->array[right_child].key){
				least_child = right_child;
			}
		}


		// check to see if we need to swap nodes.
		if( self->array[index].key > self->array[least_child].key){
			// swap the nodes.
			void* data = self->array[index].data;
			int key = self->array[index].key;
			self->array[index].data = self->array[least_child].data;
			self->array[index].key = self->array[least_child].key;
			self->array[least_child].data = data;
			self->array[least_child].key = key;

			index = least_child;
			left_child = index*2;
			right_child = index*2+1;
			continue;
		}
		
		break;
	}

	return return_data;
}

void* PQueue_peek(PQueue_t* self){
	if( self->size == 1 ){return 0;}
	return self->array[1].data;
}

int PQueue_size(PQueue_t* self){
	return self->size -1;
}


int main(){
	PQueue_t* pq = create_PQueue_t(8);
	if(!pq){exit(1);}

	int data[8] = {1,2,3,4,5,6,7,8};
	int key[8] =  {4,5,6,1,2,3,7,8};
	int i = 0;
	for(i = 0; i < 8; ++i){
		printf("%d \n", PQueue_insert(pq,(void*) &data[i],data[i]) );
	}

	for( i = 0; i < 8 ; ++i){
		printf("in: %d\n",pq->array[i].key);
	}

	printf("size = %d\n",PQueue_size(pq));
	printf("top = %d\n",*((int*)PQueue_peek(pq)) );

	for( i = 0; i < 8; ++i){
		int* rs = (int*) PQueue_remove(pq);
		if(rs != 0){
			printf("remove = %d\n",*rs);
			
		}else{
			printf("remove failed\n");
		}
		printf("size = %d\n", pq->size);
	}
	
	return 0;
}