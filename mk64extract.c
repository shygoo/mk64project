#include "sg2.h"
#include "mio0decode.h"

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;



typedef struct {
	u32 mio0_a_start; // dlists and ?
	u32 mio0_a_end;
	u32 mio0_b_start; // ?
	u32 mio0_b_end;
	u32 file_unk_a;
	u32 file_unk_b;
	u32 unk1;
	u32 unk2;
	u32 unk3;
	u32 unk4;
	u32 unk5;
	u16 unk6;
	u16 padding;
} LevelEntry;

int main(int argc, char* argv[]){
	u32 romsize;
	void* rom = loadfile(argv[1], &romsize);
	LevelEntry * entries = rom + 0x122390;
	char output_filename_a[40];
	char output_filename_b[40];
	
	mkdir(argv[2], 0700);
	
	for(int i = 0; i < 20; i++){
		
		void* mio0_block_a = rom + swap32(entries[i].mio0_a_start);
		void* mio0_block_b = rom + swap32(entries[i].mio0_b_start);
		
		u32 decoded_a_size;
		u32 decoded_b_size;
		
		void* decoded_a = mio0decode(mio0_block_a, &decoded_a_size);
		void* decoded_b = mio0decode(mio0_block_b, &decoded_b_size);

		sprintf(output_filename_a, "%s\\Level_%02d_A.bin\0", argv[2], i);  
		sprintf(output_filename_b, "%s\\Level_%02d_B.bin\0", argv[2], i);  
		
		writefile(output_filename_a, decoded_a, decoded_a_size);
		writefile(output_filename_b, decoded_b, decoded_b_size);
		
		printf("Level %02d : %08X (%05X), %08X (%05X)\n", i, swap32(entries[i].mio0_a_start), decoded_a_size, swap32(entries[i].mio0_b_start), decoded_b_size);
		
		free(decoded_a);
		free(decoded_b);
	}
	//system("pause");
}