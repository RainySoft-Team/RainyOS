#ifndef _INT32_H_
#define _INT32_H_

#include "types.h"
#include "common.h"

typedef struct {
    uint16_t di, si, bp, sp, bx, dx, cx, ax;
    uint16_t gs, fs, es, ds, flags;
} regs16_t;

void _int32(uint8_t intnum, regs16_t *regs);

#endif
