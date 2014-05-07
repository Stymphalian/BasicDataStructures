
/* encode the string into its binary format */
char* encode(char* string){
	if( !string) {return NULL;}
	if( strlen(string) %2  != 0 ){return NULL;}

	char* hashcode = malloc((strlen(string)/2)+1); /* each digit represents 4 bits, therefore every 2 is a byte */
	memset(hashcode, 0, strlen(string)/2  +1 );

	int pos = 0;
	int i = 0;

	char temp=0;
	char temp2=0;
	for( i = 0; i < strlen(string); i+=2){
		if( string[i] >= '0' && string[i] <= '9'){
			temp = string[i] -'0';
		}else if( (string[i] >= 'a' && string[i] <= 'f')){
			temp = string[i] - 'a' +10;
		}else if ((string[i] >= 'A' && string[i] <= 'F') ){
			temp = string[i] - 'A' +10;
		}else{
			break;
		}

		if( string[i+1] >= '0' && string[i+1] <= '9'){
			temp2 = string[i+1] - '0';
		}else if( (string[i+1] >= 'a' && string[i+1] <= 'f')){
			temp2 = string[i+1] - 'a'+10;
		}else if ((string[i+1] >= 'A' && string[i+1] <= 'F') ){
			temp2 = string[i+1] - 'A'+10;
		}else{
			break;
		}

		temp2 = temp2 << 4;
		temp2 = temp2 | temp;
		memset(hashcode+pos,temp2,1);
		pos++;
	}
	return hashcode;
}

/* decode the bit hashcode into a hexadecimal string */
/* assumption that len is divisble by 4 */
/* len in bits */

char* decode(void* _hashcode, int len){
	if(len%4 != 0 ){return NULL;}
	if(!_hashcode) {return NULL;}
	
	unsigned char* hashcode = (unsigned char*) _hashcode;
	char* string = malloc(sizeof(char)*((len/4)+1));

	int i =0;
	unsigned char low;
	unsigned char high;


	int pos = 0;
	for( i =0 ; i < len/8; ++i){
		/*low = *(hashcode + i);  read 1 byte of input 
		high = low >> 4;
		low = low & 0x0f;
		*/
		high = *(hashcode + i);
		low = high >>4;
		high = high & 0x0f;

	
		if( low >= 0 && low <= 9){
			string[pos++] = low + '0';
		}else{
			string[pos++] = low -10 + 'a';
		}

		if( high >= 0 && high <= 9){
			string[pos++] = high + '0';
		}else{
			string[pos++] = high -10 +'a';
		}
	}

	string[len/4] = '\0';	

	return string;
}


