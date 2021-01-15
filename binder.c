#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "binder.h"

/*
    Return size of file
*/
long getFileSize(FILE *handle)
{
    long iRet = 0;
    fseek(handle, 0L, SEEK_END);
    iRet = ftell(handle);
    fseek(handle, 0L, SEEK_SET);

    return iRet;
}

/*
    Generate a random key with specified size
*/
Error getRandomKey(byte **keyPointer, int size)
{
    byte *randomKey;
    int i;
    Error ret = SUCCESS;

    srand(time(NULL));
    randomKey = (byte*)calloc(size, sizeof(byte));

    if(randomKey)
    {
        PDEBUG("\nrandomKey = ");
        for(i=0;i<size;i++)
        {
            randomKey[i] = rand() % 0xFF;
            PDEBUG("%x\t",randomKey[i]);
        }
        PDEBUG("\n");
        *(keyPointer) = randomKey;
    }
    else
    {
        ret = ALLOC_ERR;
    }

    return ret;
}

/*
    Encrypt a input with XOR key
*/
Error XOREncrypt(byte *input, byte *key, long sizeInput, int sizeKey)
{
    int i;
    Error ret = SUCCESS;

    //PDEBUG("\nsizeInput = %d\nsizeKey = %d\nKey = ", sizeInput, sizeKey);
    DEBUG("\nsizeInput = %d\nsizeKey = %d\nKey = ", sizeInput, sizeKey);
    PDEBUG_HEX("%x\t", key, sizeKey);
    for(i=0;i<sizeInput;i++)
    {
        input[i] ^= key[i % sizeKey];
    }

    return ret;
}



/*
    Join two files inside a stub file
*/
Error JoinFiles(const char *file1, const char *file2, const char *stub)
{
    FILE *fileHndl[2];
    FILE *outputHndl;
    long sizeFile[2];
    byte *buffer[2];
    byte *randomKey;
    Error iRet = SUCCESS;
    byte keyToken[KEY_TOKEN_SIZE + KEY_TOKEN_PADDING];
    byte splitToken[SPLIT_TOKEN_SIZE + SPLIT_TOKEN_PADDING];

    memset(splitToken, 0x00, SPLIT_TOKEN_SIZE + SPLIT_TOKEN_PADDING);
    memset(keyToken, 0x00, KEY_TOKEN_SIZE + KEY_TOKEN_PADDING);

    outputHndl = fopen(stub, "ab");
    if(outputHndl)
    {
        fileHndl[0] = fopen(file1, "rb");
        fileHndl[1] = fopen(file2, "rb");
        PDEBUG("%p - %p \n", fileHndl[0], fileHndl[1]);
        if(fileHndl[0] && fileHndl[1])
        {
                
            sizeFile[0] = getFileSize(fileHndl[0]);
            sizeFile[1] = getFileSize(fileHndl[1]);
            
            buffer[0] = (byte*)calloc(sizeFile[0], 1L);
            buffer[1] = (byte*)calloc(sizeFile[1], 1L);
            
            if(buffer[0] && buffer[1])
            {
                
                fread(buffer[0], 1L, sizeFile[0], fileHndl[0]);
                fread(buffer[1], 1L, sizeFile[1], fileHndl[1]);
                if(getRandomKey(&randomKey, KEY_SIZE) == SUCCESS)
                {                    
                    XOREncrypt(buffer[0], randomKey, sizeFile[0], KEY_SIZE);
                    XOREncrypt(buffer[1], randomKey, sizeFile[1], KEY_SIZE);

                    memcpy(&keyToken[KEY_TOKEN_PADDING], KEY_TOKEN, KEY_TOKEN_SIZE);
                    memcpy(&splitToken[SPLIT_TOKEN_PADDING], SPLIT_TOKEN, SPLIT_TOKEN_SIZE);

                    fwrite(keyToken, 1L, KEY_TOKEN_SIZE + KEY_TOKEN_PADDING, outputHndl);
                    fwrite(randomKey, 1L, KEY_SIZE, outputHndl);
                    fwrite(splitToken, 1L, SPLIT_TOKEN_SIZE + SPLIT_TOKEN_PADDING, outputHndl);
                    
                    fwrite(buffer[0], 1L, sizeFile[0], outputHndl);  
                   
                    fwrite(splitToken, 1L, SPLIT_TOKEN_SIZE + SPLIT_TOKEN_PADDING, outputHndl);
                    fwrite(buffer[1], 1L, sizeFile[1], outputHndl); 
                    free(randomKey);
                }
                free(buffer[0]);
                free(buffer[1]);
            }
            else
            {
                iRet = ALLOC_ERR;
            }
            fclose(fileHndl[0]);
            fclose(fileHndl[1]);
        }
        else
        {
            iRet = FILE_ERR;
        }
    }
    else
    {
        iRet = FILE_ERR;
    }

    fclose(outputHndl);
    return iRet;
}

/*
    Find block of memmory in a buffer
*/
byte* findBlockMem(const byte *initPtr, byte *endPtr, const byte *block, long sizeBlock)
{
    byte *ret = NULL;
    byte *pos;
    long i;
    for(pos=initPtr;pos != endPtr; pos++)
    {
        if(memcmp(pos, block, sizeBlock) == 0)
        {
            ret = pos;
            break;
        }
    }

    return ret;
}


/*
    Find token in buffer input
*/
Error parseBuffer(const byte* input, Splited *files, long size)
{
    int i = 0;
    int j = 0;
    long tokenKeySize = KEY_TOKEN_SIZE + KEY_TOKEN_PADDING;
    long splitTokenSize = SPLIT_TOKEN_SIZE + SPLIT_TOKEN_PADDING;
    byte *keyPos = NULL;
    byte *programPos = NULL;
    byte *aux = NULL;
    byte *endPtr = &input[size];
    Error ret = SUCCESS;
    byte* tokenKey;
    byte* splitToken;
    

    tokenKey = calloc(1, tokenKeySize);
    splitToken = calloc(1, splitTokenSize);

    if(tokenKey && splitToken)
    {
        memcpy(&tokenKey[KEY_TOKEN_PADDING], KEY_TOKEN, KEY_TOKEN_SIZE);
        memcpy(&splitToken[SPLIT_TOKEN_PADDING], SPLIT_TOKEN, SPLIT_TOKEN_SIZE);

        PDEBUG("Token Key = ");
        PDEBUG_HEX("%x\t", tokenKey, tokenKeySize);

        keyPos = findBlockMem(input, endPtr, tokenKey, tokenKeySize);

        if(keyPos != NULL)
        {
            PDEBUG("Key found on pos %d\n", (keyPos-input));

            keyPos = (keyPos + tokenKeySize);

            programPos = findBlockMem(keyPos, endPtr, splitToken, splitTokenSize);
            if(programPos != NULL)
            {
                PDEBUG("Program 1 found on %ld\nKey = ", (programPos - input));
                PDEBUG_HEX("%x\t", keyPos, (programPos-keyPos));

                files->sizeKey = (programPos-keyPos);
                files->key = calloc(1, files->sizeKey);
                memcpy(files->key, keyPos, files->sizeKey);

                programPos = (programPos + splitTokenSize);
                aux = programPos;

                programPos = findBlockMem(programPos, endPtr, splitToken, splitTokenSize);
                if(programPos != NULL)
                {
                    PDEBUG("Program 2 found on %ld\n", (programPos - input));
                    
                    files->sizeFile[0] = (programPos - aux);
                    PDEBUG("1\n");
                    files->fileBuffer[0] = calloc(1, (programPos - aux));
                    PDEBUG("2\n");
                    memcpy(files->fileBuffer[0], aux, (programPos - aux));
                    PDEBUG("3\n");
                    programPos = (programPos + splitTokenSize);
                    PDEBUG("4\n");
                    files->sizeFile[1] = (endPtr - programPos);
                    PDEBUG("5\n");
                    
                    files->fileBuffer[1] = calloc(1, (endPtr - programPos));
                    PDEBUG("6\n");
                    
                    memcpy(files->fileBuffer[1], programPos, (endPtr - programPos));
                    PDEBUG("7\n");
                }
                else
                {
                    ret = PROGRAM_TOKEN_NOT_FOUND;
                }
            }
            else
            {
                ret = PROGRAM_TOKEN_NOT_FOUND;
            }
            
        }
        else
        {
            ret = KEY_TOKEN_NOT_FOUND;
        }
        PDEBUG("8\n");

    }
    else
    {
        ret = ALLOC_ERR;
    }
    PDEBUG("11\n");
    return ret;

}

/* Split files in stub */
Error splitFiles(const char *stub, const char fileName[][200])
{
    FILE *fileHndl;
    FILE *outp[2];
    Error ret = SUCCESS;
    long size = 0;
    byte *buffer;

    fileHndl = fopen(stub, "rb");
    if(fileHndl)
    {
        Splited *files;
        files = calloc(1, sizeof(files));
        if(files)
        {
            size = getFileSize(fileHndl);
            buffer = calloc(1, size);
            if(buffer)
            {
                fread(buffer, 1L, size, fileHndl);
                //PDEBUG_HEX("%x\t", buffer, size);
                ret = parseBuffer(buffer, files, size);
                PDEBUG("parseBuffer ret = %d\n", ret);
                if(ret == SUCCESS)
                {
                    free(buffer);
                    int i;
                    for(i=0;i<2;i++)
                    {
                        //char filename[10];
                        //sprintf(filename, "file%d.out", i);
                        PDEBUG("Writing on %s\n", fileName[i]);
                        outp[i] = fopen(fileName[i], "wb");
                        //UnRotN(files->fileBuffer[i], ROTN, files->sizeFile[i]);
                        if(outp[i])
                        {
                            XOREncrypt(files->fileBuffer[i], files->key, files->sizeFile[i], files->sizeKey);
                            fwrite(files->fileBuffer[i], 1L, files->sizeFile[i], outp[i]);
                            fclose(outp[i]);
                        }
                        else
                        {
                            ret = FILE_ERR;
                            break;
                        }
                    }
                }
                free(buffer);
            }
            else
            {
                ret = ALLOC_ERR;
            }
            free(files);
        }
        else
        {
            ret = ALLOC_ERR;
        }
    }
    else
    {
        ret = FILE_ERR;
    }

    return ret;
    
}
