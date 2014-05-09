#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>

#include "avlnode.h"
#include "avltree.h"


int getline(char** line, unsigned* linesize, FILE* file);

int main(int argc,char** argv){
	if ( argc != 2 ){printf("%s: [file]\n", argv[0]);return 1; };
	char* line = 0;
	unsigned linesize = 0;
	int len = 0;	
	FILE* file = fopen(argv[1],"r");


	avltree<int,int>* tree1 = new avltree<int,int>();
	#define m_cmp(a) ( strncmp(a,line,strlen(a)) == 0)
	while( (len = getline(&line,&linesize,file)) != -1 ){
		line[len-1] = 0;
		if( m_cmp("insert")){
			int num1 = 0;
			int num2 = 0;
			if( sscanf(line,"insert %d %d",&num1,&num2) != 2 ){continue;}
			printf("insert %d %d = %d\n", num1,num2, tree1->insert(num1,num2));
		} else if( m_cmp("remove") ){
			int key = 0;
			if( sscanf(line,"remove %d", &key) != 1 ) {continue;}
			printf("remove %d = %d\n",key,tree1->remove(key));
		} else if( m_cmp("find") ){
			int key = 0;
			if( sscanf(line,"find %d",&key) != 1 ){continue;}
			printf("find %d = %d\n",key,tree1->find(key));
		} else if( m_cmp("print" ) ){
			int type  =0;
			if( sscanf(line,"print %d", &type)  != 1) {continue;}

			printf("print start \n");
			std::list<int>* list1 = tree1->traverse(type);
			std::list<int>::iterator it;
			int here = 0;
			for( it = list1->begin(); it != list1->end(); ++it){
				here =1;
				printf("%d ", (*it));
			}
			if( here == 1 ){printf("\n");}
			printf("print end\n");
			delete list1;
		} else if( m_cmp("size") ){
			printf("size = %d\n", tree1->getSize() );
		} else if( m_cmp("height") ){
			printf("height = %d\n",tree1->height() );			
		} else if( m_cmp("reset") ){
			printf("reset\n");
			delete tree1;
			printf("\n");
			tree1 = new avltree<int,int>();
		} else if( m_cmp("isempty" )) {
			printf("isempty = %d\n", tree1->isEmpty() );
		} else if (m_cmp("compare" ) ){
			int count = 0;
			int type = 0;
			int num = 0;
			int amount = 0;
			char str[32];

			if( sscanf(line,"compare %d %d",&type,&count) != 2) {continue;}
			printf("compare start\n");


			std::list<int>* list1 = tree1->traverse(type);
			std::list<int>::iterator it;
			for( it = list1->begin(); it != list1->end(); ++it){
				printf("%d ", *it);
			}printf("\n");

			amount += strlen("compare ");
			sprintf(str,"%d",type);
			amount += strlen(str) +1;
			sprintf(str,"%d",count);
			amount += strlen(str) +1;
	
			int difference = 0;
			if( (unsigned) count != list1->size() ){difference =1 ;}
			it = list1->begin();
			while( count-- && it != list1->end() ){
				if( sscanf(line+ amount, "%d",&num) != 1 ){continue;};
				printf("%d ", num);
				if( (*it) != num ) { difference = 1;}

				sprintf(str,"%d",num);
				amount += strlen(str) +1;
				++it;
			}printf("\n");

			if( difference == 1 ){
				printf("Differences found.\n");
			}
			printf("compare end\n");
			delete list1;
		}else if ( m_cmp("graph") ){
			tree1->formatted_print();
		}
	}
	#undef m_cmp

	printf("--------------- done ---------------\n");
	free(line);
	fclose(file);
	delete tree1;
	return 0;
}

int getline (char** line,size_t* lineSize, FILE* file){
	if( line == NULL || file == NULL ) { return -1;}	
	if( *line == NULL || *lineSize == 0 ){
		if( *lineSize == 0 ){
			/*int defaultSize = 25;*/
			*lineSize = (size_t) 25;
			free(*line);
		}
		*line = (char*) malloc(sizeof(char)* (*lineSize ));
		if(!(*line)) { return -1; }
	}

	unsigned count =0;
	char c =0;

	while( (c = fgetc(file)) != EOF) {
		if( count >= (*lineSize)-1 ){
			(*line) = (char*) realloc((*line),sizeof(char)* ((*lineSize) *2));
			if(!(*line)) {return -1;}
			*lineSize =  (*lineSize)*2;	
		}	
		(*line)[count++]=c;
		if( c == '\n') {break;}	
	}
	(*line)[count] = '\0';
	
	if(count == 0){ return -1;}	
	else{ return count;}
}
