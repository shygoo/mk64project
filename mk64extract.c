#include "sg2.h"
#include "mio0decode.h"

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;


char* levelnames[] = {
	"MARIORACEWAY",     // 00
	"CHOCOMOUNTAIN",    // 01
	"BOWSERSCASTLE",    // 02
	"BANSHEEBOARDWALK", // 03
	"YOSHIVALLEY",      // 04
	"FRAPPESNOWLAND",   // 05
	"KOOPATROOPABEACH", // 06
	"ROYALRACEWAY",     // 07
	"LUIGIRACEWAY",     // 08
	"MOOMOOFARM",       // 09
	"TOADSTURNPIKE",    // 0A
	"KALIMARIDESERT",   // 0B
	"SHERBETLAND",      // 0C
	"RAINBOWROAD",      // 0D
	"WARIOSTADIUM",     // 0E
	"BLOCKFORT",        // 0F
	"SKYSCRAPER",       // 10
	"DOUBLEDECK",       // 11
	"JUNGLEPARKWAY",    // 12
	"BIGDONUT",         // 13
};


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
	char output_filename_a[60];
	char output_filename_b[60];
	
	mkdir(argv[2], 0700);
	
	for(int i = 0; i < 20; i++){
		void* mio0_block_a = rom + swap32(entries[i].mio0_a_start);
		void* mio0_block_b = rom + swap32(entries[i].mio0_b_start);
		
		u32 decoded_a_size;
		u32 decoded_b_size;
		
		void* decoded_a = mio0decode(mio0_block_a, &decoded_a_size);
		void* decoded_b = mio0decode(mio0_block_b, &decoded_b_size);

		sprintf(output_filename_a, "%s\\%02d_%s_A.bin\0", argv[2], i, levelnames[i]);  
		sprintf(output_filename_b, "%s\\%02d_%s_B.bin\0", argv[2], i, levelnames[i]);  
		
		writefile(output_filename_a, decoded_a, decoded_a_size);
		writefile(output_filename_b, decoded_b, decoded_b_size);
		
		printf("Level %02d %-16s %08X (%05X), %08X (%05X)\n", i, levelnames[i], swap32(entries[i].mio0_a_start), decoded_a_size, swap32(entries[i].mio0_b_start), decoded_b_size);
		
		free(decoded_a);
		free(decoded_b);
	}
}