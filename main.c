#include <stdio.h>
#include <pico/stdlib.h>
#include "include/ILI9341.h"

int main(){
    stdio_init_all();
    sleep_ms(2000);
    
    ILI9341_init(HORIZONTAL);

    ILI9341_demo();

    return 0;
}
