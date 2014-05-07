#include <vector>

struct object_id{
	int num;
	int version;
	bool operator== ( const object_id&  rhs){
		return ( num == rhs.num ) && ( version == rhs.version);
	}
};

struct object{
	object() {}
	object(object_id id): id(id) {}
	object_id id;
};

template <class T>
class slotmap {
public:
	// variables
	static const int chunk_size = 256;
	std::vector<object*> object_table;
	std::vector<object_id> free_list;

	//costructors
	slotmap(){}
	virtual ~slotmap(){
		for(unsigned i =0; i < object_table.size(); ++i){
			delete[] object_table[i];
		}
	}
	
	// methods
	object* getObject(object_id id){
		object* obj =  &( object_table[id.num/chunk_size][id.num % chunk_size] );
		return (obj->id == id) ? obj : 0;
	}

	object_id createObject(){
		if( free_list.empty() ){
			object* chunk = new object[chunk_size];
			for( int i = chunk_size-1; i >= 0 ; --i){
				chunk[i].id.num = object_table.size()*chunk_size + i;
				chunk[i].id.version = 0;
				free_list.push_back(chunk[i].id);	
			}
			object_table.push_back(chunk);
		}

		object_id new_free = free_list.back();
		free_list.pop_back();
		return object_table[new_free.num/chunk_size][new_free.num%chunk_size].id;
	}

	void destroyObject(object_id id) {
		object* obj = getObject(id);
		obj->id.version  = obj->id.version + 1;
		free_list.push_back(obj->id);	
	}


private:
};
