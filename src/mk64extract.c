#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "mio0decode.h"
#include "utils.h"

#define ROM_COURSETABLE 0x122390
#define ROM_M64BANK     0xBC5F60

char const * const levelnames[] = {
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

static void loadLevelEntries(char *rom, LevelEntry* entries, unsigned count)
{
	unsigned i;
	for (i = 0; i < count; i++) {
		entries[i].mio0_f3d_start  = BE32(&rom[0x00 + i * 0x30]);
		entries[i].mio0_f3d_end    = BE32(&rom[0x04 + i * 0x30]);
		entries[i].mio0_mesh_start = BE32(&rom[0x08 + i * 0x30]);
		entries[i].mio0_mesh_end   = BE32(&rom[0x0C + i * 0x30]);
		entries[i].unk_start       = BE32(&rom[0x10 + i * 0x30]);
		entries[i].unk_end         = BE32(&rom[0x14 + i * 0x30]);
		entries[i].unk1            = BE32(&rom[0x18 + i * 0x30]);
		entries[i].unk2            = BE32(&rom[0x1C + i * 0x30]);
		entries[i].unk3            = BE32(&rom[0x20 + i * 0x30]);
		entries[i].unk4            = BE32(&rom[0x24 + i * 0x30]);
		entries[i].unk5            = BE32(&rom[0x28 + i * 0x30]);
		entries[i].unk6            = BE16(&rom[0x2C + i * 0x30]);
		entries[i].padding         = BE16(&rom[0x2E + i * 0x30]);
	}
}

static void loadM64Entries(char *rom, M64Entry* entries, unsigned count)
{
	unsigned i;
	for (i = 0; i < count; i++) {
		entries[i].offset  = BE32(&rom[0x00 + i * 0x8]);
		entries[i].size    = BE32(&rom[0x04 + i * 0x8]);
	}
}

int main(int argc, char* argv[]){
	char output_filename_f3d[FILENAME_MAX];
	char output_filename_mesh[FILENAME_MAX];
	char output_filename_unk[FILENAME_MAX];
	char output_filename_pdlist[FILENAME_MAX];
	char output_filename[FILENAME_MAX];
	LevelEntry entries[COUNT(levelnames)];
	M64Entry* m64entries;
	char* rom_path;
	char* output_directory;
	char* rom;
	u16 m64count;
	int romsize;
	unsigned i;

	if (argc < 3) {
		fprintf(stderr, "Usage: mk64extract <ROM_PATH> <OUTPUT_DIR>\n");
		return EXIT_FAILURE;
	}
	
	rom_path         = argv[1];
	output_directory = argv[2];
	
	rom = loadfile(rom_path, &romsize);
	loadLevelEntries(&rom[0x122390], entries, COUNT(levelnames));
	
	mkdir(output_directory, 0700);
	
	// dump data referenced by course table:
	
	for(i = 0; i < COUNT(levelnames); i++){
		void* mio0_block_f3d  = &rom[entries[i].mio0_f3d_start];
		void* mio0_block_mesh = &rom[entries[i].mio0_mesh_start];
		void* block_unk       = &rom[entries[i].unk_start];
		u32 pdlist_addr = entries[i].mio0_mesh_start + (entries[i].unk3 & 0x00FFFFFF);
		void* pdlist = &rom[pdlist_addr];

		int decoded_f3d_size;
		int decoded_mesh_size;
		u32 raw_unk_size = entries[i].unk_end - entries[i].unk_start;
		u32 pdlist_size = entries[i].mio0_mesh_end - pdlist_addr;

		void* decoded_f3d  = mio0decode(mio0_block_f3d, &decoded_f3d_size);
		void* decoded_mesh = mio0decode(mio0_block_mesh, &decoded_mesh_size);

		sprintf(output_filename_f3d,  "%s/%02d_%s_SEG06.bin",  output_directory, i, levelnames[i]);
		sprintf(output_filename_mesh, "%s/%02d_%s_SEG04.bin", output_directory, i, levelnames[i]);
		sprintf(output_filename_unk,  "%s/%02d_%s_REFS.bin",  output_directory, i, levelnames[i]);
		sprintf(output_filename_pdlist, "%s/%02d_%s_SEG07.bin", output_directory, i, levelnames[i]);

		writefile(output_filename_f3d,  decoded_f3d,  decoded_f3d_size);
		writefile(output_filename_mesh, decoded_mesh, decoded_mesh_size);
		writefile(output_filename_unk,  block_unk, raw_unk_size);
		writefile(output_filename_pdlist, pdlist, pdlist_size);

		printf(
			"%02d %-16s\n  F3D  %08X : %08X (mio0)\n  MESH %08X : %08X (mio0)\n  REFS %08X : %08X\n  1 %08X\n  2 %08X\n  3 %08X\n  4 %08X\n  5 %08X\n  6 %04X\n\n",
			i,
			levelnames[i],
			entries[i].mio0_f3d_start,
			entries[i].mio0_f3d_end,
			entries[i].mio0_mesh_start,
			entries[i].mio0_mesh_end,
			entries[i].unk_start,
			entries[i].unk_end,
			entries[i].unk1,
			entries[i].unk2,
			entries[i].unk3,
			entries[i].unk4,
			entries[i].unk5,
			entries[i].unk6
		);

		free(decoded_f3d);
		free(decoded_mesh);
	}

	// dump m64 data:
	m64count = BE16(&rom[0xBC5F62]);
	m64entries = malloc(m64count * sizeof(*m64entries));
	loadM64Entries(&rom[0xBC5F64], m64entries, m64count);
	for(int i = 0; i < m64count; i++){
		sprintf(output_filename, "%s/TUNE%02d.m64", output_directory, i);
		printf("%s %08X\n", output_filename, m64entries[i].offset);
		writefile(output_filename, &rom[0xBC5F60] + m64entries[i].offset, m64entries[i].size);
	}

	// full mio0 dump:
	printf("Full MIO0 dump...\n");
	sprintf(output_filename, "%s/mio0", output_directory);
	mkdir(output_filename, 0700);
	u8 MIO0_MAGIC[] = {'M', 'I', 'O', '0'};
	for(i = 0; i < (unsigned)romsize - 4; i += 4){
		if (0 == memcmp(rom + i, MIO0_MAGIC, sizeof(MIO0_MAGIC))) {
			sprintf(output_filename, "%s/mio0/%08X.bin", output_directory, i);
			int decoded_size;
			void* decoded_data = mio0decode(rom + i, &decoded_size);
			writefile(output_filename, decoded_data, decoded_size);
			free(decoded_data);
		}
	}
	return EXIT_SUCCESS;
}
