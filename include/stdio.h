#ifndef stdio
#define stdio 0

#define LEFT 1
#define RIGHT 0

#include "vargs.h"
int printf(const char *format, ...);

int vsprintf(char *buffer,const char *format,va_list args);
#endif