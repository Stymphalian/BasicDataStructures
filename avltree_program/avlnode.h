#ifndef AVLNODE_H
#define AVLNODE_H
#include <cstdio>

template <class K, class T>
class avlnode{
	public:
		//variables
		avlnode<K,T>* left;
		avlnode<K,T>* right;
		avlnode<K,T>* parent;
		K key;
		T element;

		// methods
		avlnode(){init(0,0,0,0,0);}
		avlnode(avlnode<K,T>* p, avlnode<K,T>* l,avlnode<K,T>* r,K k, T e){ init(p,l,r,k,e);}
		~avlnode(){parent = 0; left = 0; right= 0; key = 0; element = 0;}
		void init(avlnode<K,T>* p, avlnode<K,T>* l,avlnode<K,T>* r,K k, T e){
			parent = p;
			left =l;
			right = r;
			key = k;
			element = e;
		}

	private:

};

#endif
