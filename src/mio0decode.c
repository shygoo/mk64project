// MIO0 decoder by shyguyhex 2015
// usage:
//
// void* mio0decode (
//   void * source,       // big endian mio0 block
//   u32  * ret_size      // size of decompressed data
// );
//
// creates a buffer for decompressed data and returns a pointer to it

#include <stdlib.h>

#include "mio0decode.h"
#include "utils.h"

void* mio0decode(void * source, int * ret_size){
	unsigned int target_size = BE32(source+0x04);
	void * dest = malloc(target_size);
	*ret_size = target_size;
	void * destination = dest;
	void * destination_end =  destination + target_size;
	void * offset1 = source + BE32(source+0x08);
	void * offset2 = source + BE32(source+0x0C);
	void * offset0 = source + 0x10;
	unsigned short t2_s = 0;
	char t2_c = 0;
	unsigned char t3 = 0;
	void * t1;
	int t0 = 0;
	char a2 = 0;
	while(destination < destination_end){
		if(a2 == 0){
			t0 = BE32(offset0);
			a2 = 32;
			offset0 += sizeof(int);
		}
		if(t0 >= 0x00000000){
			t2_s = BE16(offset1);
			offset1 += sizeof(unsigned short);
			t3 = t2_s >> 12;
			t2_s &= 0x0FFF;
			t1 = destination - t2_s;
			t3 += 3;
			while(t3 != 0){
				t2_c = *(char*)(t1 - 1);
				t3--;
				t1++;
				*(char*)(destination) = t2_c;
				destination++;
			}
		} else {
			t2_c = *(char*)(offset2);
			offset2++;
			*(char*)(destination) = t2_c;
			destination++;
		}
		t0 <<= 1;
		a2--;
	}
	return dest;
}

