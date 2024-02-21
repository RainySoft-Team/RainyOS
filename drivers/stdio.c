#include <stdio.h>
#include <stdarg.h>
#include "console.h"
#include "types.h"
#include <string.h>


void printf(const char *format, ...){
    va_list args;
    va_start(args, format);
    char buf[1024];
    vsprintf(buf,sizeof(buf),format,args);
    va_end(args);
    console_write(buf);
    return 0;
}
