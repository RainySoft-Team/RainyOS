#ifndef INCLUDE_VARGS_H_
#define INCLUDE_VARGS_H_

//typedef __builtin_va_list va_list;

typedef char* va_list;

//#define va_start(ap, last)         (__builtin_va_start(ap, last))
//#define va_arg(ap, type)           (__crt_va_arg(ap, type))
//#define va_end(ap) 


#define __crt_va_arg(ap, t)                                               \
        ((sizeof(t) > sizeof(__int64) || (sizeof(t) & (sizeof(t) - 1)) != 0) \
            ? **(t**)((ap += sizeof(__int64)) - sizeof(__int64))             \
            :  *(t* )((ap += sizeof(__int64)) - sizeof(__int64)))


//-------------------------------------------------------------------
void __cdecl __va_start(va_list* , ...);
#define __crt_va_start_a(ap, x) ((void)(__va_start(&ap, x)))
#define __crt_va_start(ap, x) __crt_va_start_a(ap, x)

#define __crt_va_end(ap)        ((void)(ap = (va_list)0))
//--------------------------------------------------------------------


#define va_arg(ap, type)           (__crt_va_arg(ap, type))
#define va_start __crt_va_start
#define va_end   __crt_va_end
#define va_copy(destination, source) ((destination) = (source))

//抄代码得抄完整，我的stdio.c不然都编译不了

#endif 	// INCLUDE_VARGS_H_
