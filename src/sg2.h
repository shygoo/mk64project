#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


typedef unsigned char        u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

typedef char                 s8;
typedef short               s16;
typedef int                 s32;
typedef long long           s64;

#define SWAP16(i) ((((i) & 0xFF) << 8) | (((i) & 0xFF00) >> 8))
#define SWAP32(i) ((((i) & 0xFF000000) >> 24) | (((i) & 0xFF0000) >> 8) | (((i) & 0xFF00) << 8) | (((i) & 0xFF) << 24))
#define HI16(i) (((i) >> 16) & 0xFFFF)
#define LO16(i) ((i) & 0xFFFF)

#define COUNT(arr) (sizeof(arr) / sizeof(arr[0]))

// file manipulation

void* loadfile(char* filename, u64* ret_size){
	FILE* pFile;
	pFile = fopen(filename, "rb");
	fseek(pFile, 0, SEEK_END);
	u64 fsize = ftell(pFile);
	rewind(pFile);
	void* buffer = malloc(fsize);
	fread(buffer, (size_t)fsize, 1, pFile);
	fclose(pFile);
	*ret_size = fsize;
	return buffer;
}

void writefile(char* filename, void* buffer, u64 size){
	FILE* pFile;
	pFile = fopen(filename, "wb");
	fwrite(buffer, (size_t)size, 1, pFile);
	fclose(pFile);
}

// buf manipulation

u32 getU32BE(void* buffer, u32 offset){
	return SWAP32(*(u32*)(buffer + offset));
}

u16 getU16BE(void* buffer, u32 offset){
	return SWAP16(*(u16*)(buffer + offset));
}

void setU32BE(void* buffer, u32 offset, u32 value){
	*(u32*)(buffer + offset) = SWAP32(value);
}

void setU16BE(void* buffer, u32 offset, u16 value){
	*(u16*)(buffer + offset) = SWAP16(value);
}

// debugging

void viewhex(void* buffer, int size){
	int i;
	for(i = 0; i < size; i++){
		if(i%16 == 0){ //beginning of line
			printf("%06X %04X | ", buffer+i, i); // print real pointer+offset and current offset
		}
		printf("%02X", *(u8*)(buffer+i));
		if((i+1)%4 == 0){ // space every 4 bytes
			printf(" ");
		}
		if((i+1)%16 == 0){ // newline every 16 bytes
			printf("\n");
		}
	}
}