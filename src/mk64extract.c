#include "sg2.h"
#include "mio0decode.h"


#define ROM_COURSETABLE 0x122390
#define ROM_M64BANK     0xBC5F60

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
	u32 mio0_f3d_start; // dlist
	u32 mio0_f3d_end;
	u32 mio0_mesh_start; // mesh
	u32 mio0_mesh_end;
	u32 unk_start;
	u32 unk_end;
	u32 unk1;
	u32 unk2;
	u32 unk3;
	u32 unk4;
	u32 unk5;
	u16 unk6;
	u16 padding;
} LevelEntry;

typedef struct {
	u32 offset;
	u32 size;
} M64Entry;

int main(int argc, char* argv[]){
	u32 romsize;
	
	char* rom_path         = argv[1];
	char* output_directory = argv[2];
	
	char* rom = loadfile(rom_path, &romsize);
	LevelEntry* entries  = &rom[0x122390];
	M64Entry* m64entries = &rom[0xBC5F64];
	u16 m64count = getU16BE(rom, 0xBC5F62);
	
	char output_filename_f3d[60];
	char output_filename_mesh[60];
	char output_filename_unk[60];
	
	mkdir(output_directory, 0700);
	
	// dump data referenced by course table:
	
	for(int i = 0; i < 20; i++){
		void* mio0_block_f3d  = &rom[SWAP32(entries[i].mio0_f3d_start)];
		void* mio0_block_mesh = &rom[SWAP32(entries[i].mio0_mesh_start)];
		void* block_unk       = &rom[SWAP32(entries[i].unk_start)];
		
		u32 decoded_f3d_size;
		u32 decoded_mesh_size;
		u32 raw_unk_size = SWAP32(entries[i].unk_end) - SWAP32(entries[i].unk_start);
		
		void* decoded_f3d  = mio0decode(mio0_block_f3d, &decoded_f3d_size);
		void* decoded_mesh = mio0decode(mio0_block_mesh, &decoded_mesh_size);
		
		sprintf(output_filename_f3d,  "%s\\%02d_%s_F3D.bin\0",  output_directory, i, levelnames[i]);  
		sprintf(output_filename_mesh, "%s\\%02d_%s_MESH.bin\0", output_directory, i, levelnames[i]);  
		sprintf(output_filename_unk,  "%s\\%02d_%s_REFS.bin\0",  output_directory, i, levelnames[i]);  
		
		writefile(output_filename_f3d,  decoded_f3d,  decoded_f3d_size);
		writefile(output_filename_mesh, decoded_mesh, decoded_mesh_size);
		writefile(output_filename_unk,  block_unk, raw_unk_size);
		
		printf(
			"%02d %-16s\n  F3D  %08X : %08X (mio0)\n  MESH %08X : %08X (mio0)\n  REFS %08X : %08X\n  1 %08X\n  2 %08X\n  3 %08X\n  4 %08X\n  5 %08X\n  6 %04X\n\n",
			i,
			levelnames[i],
			SWAP32(entries[i].mio0_f3d_start),
			SWAP32(entries[i].mio0_f3d_end),
			SWAP32(entries[i].mio0_mesh_start),
			SWAP32(entries[i].mio0_mesh_end),
			SWAP32(entries[i].unk_start),
			SWAP32(entries[i].unk_end),
			SWAP32(entries[i].unk1),
			SWAP32(entries[i].unk2),
			SWAP32(entries[i].unk3),
			SWAP32(entries[i].unk4),
			SWAP32(entries[i].unk5),
			SWAP16(entries[i].unk6)
		);

		free(decoded_f3d);
		free(decoded_mesh);
	}
	
	// dump m64 data:
	
	char output_filename[60];
	for(int i = 0; i < m64count; i++){
		sprintf(output_filename, "%s\\TUNE%02d.m64", output_directory, i);
		printf("%s\n", output_filename, SWAP32(m64entries[i].offset));
		writefile(output_filename, &rom[0xBC5F60] + SWAP32(m64entries[i].offset), SWAP32(m64entries[i].size));
	}
	
	// full mio0 dump:
	printf("Full MIO0 dump...\n");
	sprintf(output_filename, "%s\\mio0", output_directory);
	mkdir(output_filename);
	u32 MIO0_MAGIC_LE = 0x304F494D; // 4D494F30
	for(u32 i = 0; i < romsize; i += 4){
		if(*(u32*)(rom + i) == MIO0_MAGIC_LE){
			sprintf(output_filename, "%s\\mio0\\%08X.bin", output_directory, i);
			u32 decoded_size;
			void* decoded_data = mio0decode(rom + i, &decoded_size);
			writefile(output_filename, decoded_data, decoded_size);
			free(decoded_data);
		}
	}
}