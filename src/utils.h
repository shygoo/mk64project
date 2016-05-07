#ifndef UTILS_H
#define UTILS_H

typedef unsigned char        u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

typedef char                 s8;
typedef short               s16;
typedef int                 s32;
typedef long long           s64;

#define BE32(buf_) (u32)((((u8*)(buf_))[0] << 24) + (((u8*)(buf_))[1] << 16) + (((u8*)(buf_))[2] << 8) + (((u8*)(buf_))[3]))
#define BE16(buf_) (u32)((((u8*)(buf_))[0] << 8) + (((u8*)(buf_))[1]))

#define COUNT(arr) (sizeof(arr) / sizeof(arr[0]))

// file manipulation

void* loadfile(char* filename, int* ret_size);

void writefile(char* filename, void* buffer, u64 size);

#endif // UTILS_H
