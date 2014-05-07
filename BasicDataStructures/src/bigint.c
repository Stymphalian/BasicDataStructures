#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bigint.h"

static void swapPointers(void* n1, void* n2,size_t len);
static int makeSureTheLongestSequenceIsFirst(bigint** n1, bigint** n2);
static int numIsBigger(char* n1, char* n2);
static int isNegative(bigint* num);
static int checkValidNumber(char* num);
static void applyNegative(bigint* self);
static int determineParity(bigint* n1, bigint* n2);

/* operate upon two bigints */
static char* _add(bigint* self, bigint* num2);
static char* _subtract(bigint* self, bigint* num2);
static char* _multiply(bigint* self, bigint* num2);
static char* _divide(bigint* self, bigint* num2);
static int _set(bigint* self, char* num);
static int _compare(bigint* self, bigint* num2);
static bigint* _copy(bigint* self);


/* operate on a bigint using a char string for the other */
/* These are basically wrapper functions for the operators which use
 * two bigint structures,More just for convenience */
static char* _add_c(bigint* self, char* num);
static char* _subtract_c(bigint* self, char* num);
static char* _multiply_c(bigint* self, char* num);
static char* _divide_c(bigint* self, char* num);
static int _compare_c(bigint* self, char* num);



bigint* create_bigint(char* num){
	if(!checkValidNumber(num) ){return NULL;}
	if(!num){return NULL;}
	bigint* newnum = malloc(sizeof(bigint));

	newnum->add = &_add;
	newnum->subtract = &_subtract;
	newnum->multiply = &_multiply;
	newnum->divide = &_divide;
	newnum->set = &_set;
	newnum->compare = &_compare;
	newnum->copy = &_copy;

	newnum->add_c = &_add_c;
	newnum->subtract_c = &_subtract_c;
	newnum->multiply_c = &_multiply_c;
	newnum->divide_c = &_divide_c;
	newnum->compare_c = &_compare_c;

	do{

		newnum->cap = strlen(num) + 1 + 1; /* \0 and -ve */
		if( num[0] == '-' ) { newnum->cap-- ; }

		newnum->number = malloc(sizeof(char)* newnum->cap);
		if(!newnum->number) { break;}
		memmove(newnum->number, num, sizeof(char)*(strlen(num) +1 ));
		newnum->number[strlen(num)] = '\0';

		return newnum;

	}while(0);

	free(newnum);
	return NULL;
}

void destroy_bigint(bigint* self){
	if(!self){return;}
	free(self->number);
	free(self);
}


static void swapPointers(void* n1, void* n2,size_t len){ /* length in bytes */
	char temp[len];
	memcpy(temp, (char*) n1,len);
	memcpy((char*)n1,(char*)n2,len);
	memcpy((char*)n2, temp, len);
}

static int makeSureTheLongestSequenceIsFirst(bigint** n1, bigint** n2){
	if( strlen((*n2)->number) > strlen((*n1)->number) ){
		swapPointers(n1,n2,sizeof(bigint*));
		return 1;
	}else if(  strlen((*n2)->number) == strlen((*n1)->number)) {
		if( numIsBigger((*n2)->number, (*n1)->number)){
			swapPointers(n1,n2,sizeof(bigint*));
			return 1;
		}
		return 0;
	}
	return 0;

}

static char* _add(bigint* self, bigint* num2){
	if(!self || !num2){return NULL;}	
	int operator = 1;
	int bothNegFlag  =0;
	int negflag = 0;
	bigint* n1 = self->copy(self);
	bigint* n2 = num2->copy(num2);


	/* get into the case where we are either:
	 * 		postitive + positive 
	 * 		postitive - positive
	 */
	if( isNegative(self) && isNegative(num2) ){
		operator = 1;
		applyNegative(n1);
		applyNegative(n2);
		bothNegFlag = 1;
	}else if( isNegative(n1) && !isNegative(n2)) {	
		operator = -1;
		applyNegative(n1);
		swapPointers(&n1,&n2,sizeof(bigint*));
		negflag = makeSureTheLongestSequenceIsFirst(&n1,&n2);
	}else if (!isNegative(self) && isNegative(num2) ){
		operator = -1;
		applyNegative(n2);
		negflag = makeSureTheLongestSequenceIsFirst(&n1,&n2);
	}else{
		operator = 1;
	}


	/* Make sure n1 array is large enough */
	if( n1->cap < n2->cap						&&
		strlen(n1->number) < strlen(n2->number) &&
		strlen(n2->number) >= n1->cap -2
	  )
	{
		int len  = strlen(n1->number);
		n1->number = realloc(n1->number, sizeof(char)* n2->cap );
		n1->cap = n2->cap;
		n1->number[len] = '\0';

	}

	/* this section:
	 * --go along from the right to left applying the operator to each digit
	 * 		of the sequences and placing the resulting value into then n1 character
	 * 		array. If there is a carry ( >= 10 for positive, or < 0 for negative )
	 * 		then it will record it and apply it in the next iteration 
	 */
	int pos1 = strlen(n1->number) -1; /* -1 because strlen(car) = 3,but the pos of r is 2 */
	int pos2 = strlen(n2->number) -1; /* -1 beacuse strlen(car) = 3,but the pos of r is 2 */
	int len = (pos1 < pos2 ) ? pos2 : pos1;
	int longest = len+1;
	n1->number[longest] = '\0';

	/*  -walk along the longest sequence and operate on both the digits
	 *  -place the result into the n1's char array
	 */
	while( len >= 0 && pos1 >= 0 && pos2 >= 0){
		if( operator >0 ){
			n1->number[len] = n1->number[pos1] + n2->number[pos2] - 2*'0';
		}else{
			n1->number[len] = (n1->number[pos1]- '0') - (n2->number[pos2] -'0');
		}
		--len;
		--pos1;
		--pos2;
	}

	/* handle the remainder of the sequence
	 * - first we must identify which of the sequences still needs to be finished off.
	 * - then iterate through that array and input the numeric value into n1
	 */
	if( pos1 < 0  && pos2 < 0){ /* Both sequences have been finished */

	}else if( pos1< 0 ){ /* n2 stills needs to be processed */
		while( len >= 0 && pos2 >= 0){
			n1->number[len] = n2->number[pos2] -'0';
			--len;
			--pos2;
		}
	
	}else {/* n1 still needs to be processed */
		while( len>= 0 && pos1 >= 0){
			n1->number[len] = (n1->number[pos1] -'0');
			--len;
			--pos1;
		}
	}


	/* This section handles all the overflow from the operations */
	int i;
	for(i = longest -1; i> 0; --i){ /* note the >, because we check index 0 specially */
		int c = n1->number[i];
		if( operator >0 ){
			if( c >= 10){
				n1->number[i-1] += 1;
				n1->number[i] = c-10;
			}
		}else{
			if( c < 0 ){
				c = -(10 + c); /* 5-8= -3, but we actually need -7, because real calc is 15 -8 = 7 */
				if( n1->number[i-1] == 0 ){
					n1->number[i-1] = -1; /* this should turn to a 9 in the end */
				}else{
					n1->number[i-1] -= 1;
				}
				n1->number[i] = -c; /* we want the positive c value */
			}
		}
	}

	if(operator > 0 ){
		int c = n1->number[0];
		if( c >= 10 ){
			/* because of the overflow we must now expand the array */
			if( longest >= n1->cap -2 ){ /* only expand if we are at the max capacity */
				n1->number= realloc(n1->number, sizeof(char) * ( n1->cap + 1) );
				n1->cap = n1->cap +1;
			}
			memmove(n1->number+1,n1->number, sizeof(char)* (longest +1));
			n1->number[0] = 1;
			n1->number[1] = c-10;
			longest++;
		}else{
			/* everything is fine and dandy */
		}
	}else{
		int c = n1->number[0];
		if(c <= 0){ /* It should only be 0 , if it <0 then you did something wrong */
			int cnt = 1;
			int n1len = longest;
			while( cnt < n1len){
				if( n1->number[cnt] == 0 ){
					++cnt;
				}else{
					break;
				}
			}
			if( cnt == n1len) { --cnt ;} /* keep atleast 1 zero to represent the number */

			/* +1 for \0, -cnt for number of front zeroes */
			memmove(n1->number, n1->number+cnt,sizeof(char)*(longest-cnt+1));
			longest -= cnt;
		}	
	}


	/* convert the n1 array back into characters */
	for( i =0; i < longest; ++i ){
		n1->number[i] += '0';
	}
	n1->number[longest] = '\0';

	/* if it was the case in which both were negative */
	if( bothNegFlag){ applyNegative(n1);}
	/* In the case in which we switched the two numbers such that
	 * the longest sequence is on top */
	if( negflag) {applyNegative(n1); }
	/* so we don't have a -0  number */
	if( isNegative(n1) && n1->number[1] == '0' ){applyNegative(n1);}	

	self->set(self, n1->number);
	destroy_bigint(n1);
	destroy_bigint(n2);
	return self->number;
}
static char* _subtract(bigint* self, bigint* num2){
	if(!self || !num2) {return NULL;}
	applyNegative(num2);
	self->add(self,num2);
	applyNegative(num2);
	return self->number;
}

static void applyNegative(bigint* self){
	if( !self) {return ;}
	if( self->number[0] == '-' ){
		/* -1 to get rid of -ve, and +1 to include the \0 */
		memmove(self->number,self->number+1, sizeof( char) *(strlen(self->number) +1-1));
	}else{
		if( self->number[0] == '0' ){return;}
		memmove(self->number+1, self->number, sizeof(char)*(strlen(self->number) +1) );
		self->number[0] = '-';
	}
}

static int isNegative(bigint* n1){
	if(!n1) {return 0;}
	return (n1->number[0] == '-' );
}


static int determineParity(bigint* n1, bigint* n2){
	if(!n1 || !n2){return 0 ;}

	if( isNegative(n1) &&  isNegative(n2) ){
		return 1;
	}else if ( isNegative(n1) && !isNegative(n2) ){
		return -1;
	}else if ( !isNegative(n1) && isNegative(n2) ){
		return -1;
	}else {
		return 1;
	}
}


static char* _multiply(bigint* self, bigint* num2){
	if(!self || !num2){return NULL;}
	int parity = determineParity(self, num2);
	if(isNegative(self) ){ applyNegative(self) ;}

	bigint* i = create_bigint("0");
	bigint* increment = create_bigint("1");
	bigint* ref = self->copy(self);
	bigint* limit = num2->copy(num2);
	if(isNegative(limit)){ applyNegative(limit); }
	self->set(self,"0");


	for(; i->compare(i,limit) < 0 ; i->add(i,increment) ){
		self->add(self, ref);
	}


	destroy_bigint(i);
	destroy_bigint(increment);
	destroy_bigint(ref);
	destroy_bigint(limit);

	if(parity >0 ){ 
		return self->number; 
	}else{
		applyNegative(self);
		return self->number;
	}
}



static char* _divide(bigint* self, bigint* num2){
	if( !self || !num2 ){return NULL;}
	if( num2->number[0] == '0' && strlen(num2->number) == 1 ){return NULL;}

	int parity = determineParity(self, num2);
	if( isNegative(self)) {applyNegative(self);}

	bigint* count = create_bigint("0");
	bigint* increment = create_bigint("1");
	bigint* ref = create_bigint(num2->number);
	if( isNegative(ref) ) { applyNegative(ref);}
	bigint* sum = ref->copy(ref);


	while( sum->compare(sum,self) <= 0 ){
		sum->add(sum,ref);
		count->add(count,increment);
	}

	self->set(self,count->number);
	destroy_bigint(sum);
	destroy_bigint(count);
	destroy_bigint(increment);
	destroy_bigint(ref);

	if( parity >= 0){
		return self->number;
	}else{
		applyNegative(self);
		return self->number;
	}
}



static int _set(bigint* self, char* num){
	if(!self){return 0;}
	char _num = '0';
	if( num == NULL) { num = &_num;}
	if(!checkValidNumber(num) ){return 0;}

	int newlen = strlen(num);	
	if( newlen >= self->cap -2){
		if( num[0] == '-' ){
			self->cap = newlen + 1; /* \0 */
		}else{
			self->cap = newlen + 1 + 1; /* \0 + -ve buffer */
		}	
		self->number = realloc(self->number, sizeof(char) * self->cap);
	}
	memmove(self->number, num, sizeof(char)* (newlen+1));
	self->number[newlen] = '\0';
	return 1;

}	
static int _compare(bigint* self, bigint* num2){
	if(!self || !num2){return 0;}
	bigint* temp = self->copy(self);
	temp->subtract(temp, num2);

	if( temp->number[0] == '0'){
		destroy_bigint(temp);
		return 0;
	}else if ( isNegative(temp) ){
		destroy_bigint(temp);
		return -1;
	}else{
		destroy_bigint(temp);
		return 1;
	}
}

static int checkValidNumber(char* num){
	if( num == NULL){return 1;}
	int len = strlen(num);
	int i = 0;
	if( num[0] == '-' ){ i = 1;}
	if( num[0] == '\0'){ return 1;}

	for(; i < len; ++i){
		if(num[i] >= '0' && num[i] <= '9' ){
			continue;
		}else{
			return 0;
		}
	}
	return 1;
}



static bigint* _copy(bigint* self){
	if(!self){return NULL;}
	bigint* num = create_bigint(self->number);
	if( num == NULL){return NULL;}
	free(num->number);

	num->cap = self->cap;
	num->number = malloc(sizeof(char)*(num->cap));
	if(!num->number){ free(num); return NULL; }
	memcpy(num->number,self->number, sizeof(char)*(self->cap));
	return num;
}


/* assumption that they are both positive */
static int numIsBigger(char* n1, char* n2){
	if(!n1 || !n2 ){ return 0;}
	int len1 = strlen(n1);
	int len2 = strlen(n2);

	if( len1 > len2 ){return 1;}
	else if(len2> len1) {return 0;}
	else{
		int i;
		for( i = 0; i< len1; ++i){
			if( n1[i] > n2[i]  ){
				return 1;
			}else if( n1[i] == n2[i] ){
				continue;
			}else{
				return 0;
			}	
		}
		return 0;
	}

}


/**
 *
 * This section is where we define the _*_c functions.
 */

static char* _add_c(bigint* self, char* num){
	if( !self) {return NULL;}
	if( !checkValidNumber(num) ){return NULL;}	

	bigint* n2 = create_bigint(num);
	char* rs =_add(self, n2);
	destroy_bigint(n2);
	return rs;
}

static char* _subtract_c(bigint* self, char* num){
	if(!self) {return NULL;}
	if(!checkValidNumber(num) ){return NULL;}

	bigint* n2 = create_bigint(num);
	char* rs = _subtract(self, n2);
	destroy_bigint(n2);
	return rs;
}

static char* _multiply_c(bigint* self, char* num){
	if(!self) {return NULL;}
	if(!checkValidNumber(num) ){return NULL;}

	bigint* n2 = create_bigint(num);
	char* rs = _multiply(self, n2);
	destroy_bigint(n2);
	return rs;
}

static char* _divide_c(bigint* self, char* num){
	if(!self) {return NULL;}
	if(!checkValidNumber(num)){return NULL;}

	bigint* n2 = create_bigint(num);
	char* rs = _divide(self,n2);
	destroy_bigint(n2);
	return rs;

}

static int _compare_c(bigint* self, char* num){
	if(!self) {return 0;}
	if( !checkValidNumber(num) ){return 0;}

	bigint* n2 = create_bigint(num);
	int rs = _compare(self, n2);
	destroy_bigint(n2);
	return rs;
}


