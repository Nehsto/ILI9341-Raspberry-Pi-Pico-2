#include <stdio.h>

#include <pico/stdlib.h>

#include "ILI9341.h"
#include "ImageReader.h"

int main(){
    stdio_init_all();
    sleep_ms(2000);
    
    ILI9341_init(HORIZONTAL);

    for (int i = 0; i < 5; i++) {
        printf("Hello number %d from test-only main!\n", i);
        sleep_ms(1000);
    }

    FATFS fs;
    printf("Mounting SD card...\n");
    FRESULT fr = f_mount(&fs, "", 1);
    printf("f_mount returned: %d\n", fr);

    //ILI9341_demo();

    return 0;
}
