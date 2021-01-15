#include <stdarg.h>

void pdebug_(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

void pdebug_hex_(char *fmt, unsigned char *bytes, long size)
{
    int i;
    for(i=0;i<size;i++)
        printf(fmt, bytes[i]);

    printf("\n");
}