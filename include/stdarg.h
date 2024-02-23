#ifndef _STDARG_H_
#define _STDARG_H_

#include "common.h"

typedef char *va_list;
// 32位用寄存器传参，因此可以这样来搞可变参数，不需要用gcc builtin
#define va_start(ap, v) ap = (va_list) &v
#define va_arg(ap, t) *((t *) (ap += 4))
#define va_end(ap) ap = NULL

#endif