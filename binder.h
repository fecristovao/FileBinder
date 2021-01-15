#ifndef _BINDER_HEADER_
#define _BINDER_HEADER_

#define KEY_TOKEN "\x40\x28\x4B\x54\x6B\x6E\x29\x40"
#define KEY_TOKEN_SIZE 8
#define KEY_TOKEN_PADDING 4
#define KEY_SIZE 12

#define SPLIT_TOKEN "\x00\x40\x28\x53\x50\x4C\x54\x54\x4B\x6E\x29\x40"
#define SPLIT_TOKEN_SIZE 12
#define SPLIT_TOKEN_PADDING 4



#ifdef _DEBUG
#include "debug.c"
#define PDEBUG(...) pdebug_(__VA_ARGS__) 
#define DEBUG(fmt , args...) \
    fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, args)
#define PDEBUG_HEX(fmt, bytes, size) pdebug_hex_(fmt, bytes, size) 
#else
#define PDEBUG(...) 
#define PDEBUG_HEX(fmt, bytes, size)
#endif

typedef enum{
    SUCCESS = 0,
    ALLOC_ERR = 1,
    FILE_ERR = 2,
    INVALID_ARGS = 3,
    KEY_TOKEN_NOT_FOUND = 4,
    PROGRAM_TOKEN_NOT_FOUND = 5
}Error;
typedef enum{
    TRUE = 0,
    FALSE
}Bool;

typedef unsigned char byte;

typedef struct {
    byte *key;
    byte *fileBuffer[2];
    long sizeFile[2];
    long sizeKey;
}Splited;

/* Functions */
long getFileSize(FILE *handle);
Error JoinFiles(const char *file1, const char *file2, const char *stub);
Error getRandomKey(byte **keyPointer, int size);
Error XOREncrypt(byte *input, byte *key, long sizeInput, int sizeKey);
Error parseBuffer(const byte* input, Splited *files, long size);
Error splitFiles(const char *stub, const char fileName[][200]);
byte* findBlockMem(const byte *initPtr, byte *endPtr, const byte *block, long sizeBlock);

#endif