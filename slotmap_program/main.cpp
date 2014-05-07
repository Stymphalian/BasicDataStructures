#include <cstdio>
#include "slotmap.h"


int main(){
	slotmap<int> sm;

	for(int i =0; i < 500; ++i){
		object_id  objid = sm.createObject();
		if( i%5 == 0 ){
			printf("%d,%d\n", objid.num, objid.version);
		}
	}

	for( int i = 0; i < 400; ++i){
		object_id objid;
		objid.num = i;
		objid.version =0;
		sm.destroyObject(objid);
	}

	for(int i =0; i < 400; ++i){
		object_id  objid = sm.createObject();
		if( i%5 == 0 ){
			printf("%d,%d\n", objid.num, objid.version);
		}
	}
	return 0;

}
