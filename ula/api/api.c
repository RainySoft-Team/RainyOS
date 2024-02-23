#include "console.h"
#include "common.h"
#include "types.h"
#include "cmos.h"
#include "acpi.h"
#include "api.h"

void api_print(char *str){
    console_write(str);
}

void api_printc(char *str, real_color_t back, real_color_t fore){
    console_write_color(str, back, fore);
}

void api_datetime(){
    get_date_time();
}

uint32_t api_input(char *buf){
    return buf;
    //Here I didn't finish. I will care it later :)
}
