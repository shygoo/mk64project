#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "utils.h"

// file manipulation

void* loadfile(char* filename, int* ret_size){
	FILE* pFile;
	pFile = fopen(filename, "rb");
	fseek(pFile, 0, SEEK_END);
	int fsize = ftell(pFile);
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

