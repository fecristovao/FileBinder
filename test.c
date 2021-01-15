#include <stdlib.h>
#include <stdio.h>
#include "binder.h"

static const char strError[][200] = {
    "All operations success",
    "malloc or calloc error",
    "File handling error",
    "Invalid Arguments",
    "Key Token not found!",
    "Program Token not found!"
};

#define PERROR(x) printf("%s\n", strError[x])

int main(int argc, char **argv)
{
    Error iRet = INVALID_ARGS;
    if(argc <= 1)
    {
        printf("Args errado\n");
    }
    else if(argv[1][0]== 'j')
    {

            printf("Juntando arquivos!\n");
            iRet = JoinFiles(argv[2], argv[3], argv[4]);
    }
    else if(argv[1][0] == 's')
    {
        char fileNames[2][200] = {"a", "b"};
        iRet = splitFiles(argv[2], fileNames);
    }
    PERROR(iRet);
    return iRet;

}