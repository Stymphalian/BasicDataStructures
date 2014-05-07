#ifndef MD5_H
#define MD5



/* Thoughts:
 *
 * I don't like that factd that I am using unsigned long long as my 64 bit holder.
 * This is unreliable and not cross platform.
 * --Edit  No more need to worry about that, I am now using two 4-byte integer
 *  		in order to represent the 64-bit count value.
 */


/* Useful Defines */
#define UINT64 unsigned long long
#define UINT32 unsigned int

/* this is the context which holds all the information needed 
 * to digest the message */
typedef struct MD_CTX {
	UINT32 state[4]; /* state[0] == A, state[1] == B, ... */
	UINT32 count[2]; /* the count of the number of bits read in so far */
} MD_CTX;


void init_context(MD_CTX* context);
void update_context(MD_CTX* context, unsigned char buffer[64]);
void update_final(MD_CTX* context,unsigned char buffer[64], UINT32 len);

#endif
