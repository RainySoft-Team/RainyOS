#ifndef INCLUDE_API_H_
#define INCLUDE_API_H_

#include "common.h"
#include "types.h"
#include "vmm.h"
#include "console.h"

void api_print(char *str);
void api_printc(char *str, real_color_t back, real_color_t fore);
void api_datetime();
uint32_t api_input(char *buf);

#endif
