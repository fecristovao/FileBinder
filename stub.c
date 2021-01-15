#include <stdlib.h>
#include <stdio.h>
#include "binder.h"
#include <unistd.h>


static void extractExecute(char *selfName, char fileOutput[][200])
{
    int ret = 0;
    ret = splitFiles(selfName, fileOutput);
    #ifdef linux
    system("./teste1");
    system("./teste2");
    #else
    system("teste1.exe");
    system("teste2.exe");
    #endif
}

int main(int argc, char **argv)
{
    #ifdef linux
    char fileNames[][200] = {"teste1", "teste2"};
    #else
    char fileNames[][200] = {"teste1.exe", "teste2.exe"};
    #endif
    if (argc >= 0)
        extractExecute(argv[0], fileNames);
}