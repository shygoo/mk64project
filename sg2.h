#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

typedef char                 s8;
typedef unsigned char        u8;
typedef short               s16;
typedef unsigned short      u16;
typedef int                 s32;
typedef unsigned int        u32;
typedef long long           s64;
typedef unsigned long long  u64;
typedef char*               str;
typedef void*               bin;



typedef struct  {
	char* data;
	u32   offset;
	u32   size;
} buffer;

bin loadfile(str filename, u64* ret_size){ // this is way better than my old one
	FILE* pFile;
	pFile = fopen(filename, "rb");
	fseek(pFile, 0, SEEK_END);
	u64 fsize = ftell(pFile);
	rewind(pFile);
	bin buffer = malloc(fsize);
	fread(buffer, (size_t)fsize, 1, pFile);
	fclose(pFile);
	*ret_size = fsize;
	return buffer;
}

void writefile(str filename, bin buffer, u32 size){
	FILE * pFile;
	pFile = fopen(filename, "wb");
	fwrite(buffer, (size_t)size, 1, pFile);
	fclose(pFile);
}

buffer* new_printbuffer(u32 buffer_size){
	buffer* p = malloc(sizeof(buffer));
	p->data = malloc(buffer_size);
	p->offset = 0;
	p->size = buffer_size;
	u32 i;
	for(i = 0; i < buffer_size; i++){
		*(p->data + i) = 0x00;
	}
	return p;
}

void printb(buffer* p, char * format_string, ...){
	va_list arglist;
	va_start(arglist, format_string);
	p->offset += vsprintf(p->data + p->offset, format_string, arglist);
	va_end(arglist);
}

void buffer_dump(char* path, buffer* b){
	writefile(path, b->data, b->offset);
}

void buffer_fulldump(char* path, buffer* b){
	writefile(path, b->data, b->size);
}

void prompt(char* str, int len){
	fgets(str,len, stdin);
	size_t ln = strlen(str) - 1;
	if(str[ln] == '\n') str[ln] = '\0';
}

void showdata(void* buffer, int size){
	int i;
	for(i = 0; i < size; i++){
		if(i%16 == 0){ //beginning of line
			printf("%06X %04X | ", buffer+i, i); // print real pointer+offset and current offset
		}
		printf("%02X", *(unsigned char*)(buffer+i));
		if((i+1)%4 == 0){ // space every 4 bytes
			printf(" ");
		}
		if((i+1)%16 == 0){ // newline every 16 bytes
			printf("\n");
		}
	}
}