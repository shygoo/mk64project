// MIO0 decoder by shyguyhex 2015
// usage:
//
// void* mio0decode (
//	 void * source,       // big endian mio0 block
//   u32  * ret_size      // size of decompressed data
// );
//
// creates a buffer for decompressed data and returns a pointer to it

typedef char                 s8;
typedef unsigned char        u8;
typedef short               s16;
typedef unsigned short      u16;
typedef int                 s32;
typedef unsigned int        u32;

u16 swap16(short s){
	unsigned short ret = 0x0000;
	ret |= (s&0xFF) << 8;
	ret |= (s >> 8) & 0x00FF;
	return ret;
}

u32 swap32(u32 num){
	u8 t0;
	u8 t1;
	u8 t2;
	u8 t3;
	u8 * data = &num;
	t0 = data[0];
	t1 = data[1];
	t2 = data[2];
	t3 = data[3];
	data[0] = t3;
	data[1] = t2;
	data[2] = t1;
	data[3] = t0;
	return *(u32*)data;
}

void* mio0decode(void * source, int * ret_size){
	unsigned int target_size = swap32(*(int*)(source+0x04));
	void * dest = malloc(target_size);
	*ret_size = target_size;
	void * destination = dest;
	void * destination_save = destination;
	void * destination_end =  destination + target_size;
	void * offset1 = source + swap32(*(int*)(source+0x08));
	void * offset2 = source + swap32(*(int*)(source+0x0C));
	void * offset0 = source + 0x10;
	unsigned short t2_s = 0;
	char t2_c = 0;
	unsigned char t3 = 0;
	void * t1;
	int t0 = 0;
	char a2 = 0;
	while(destination < destination_end){
		if(a2 == 0){
			t0 = swap32(*(int*)(offset0));
			a2 = 32;
			offset0 += sizeof(int);
		}
		if(t0 >= 0x00000000){
			t2_s = swap16(*(unsigned short*)(offset1));
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


