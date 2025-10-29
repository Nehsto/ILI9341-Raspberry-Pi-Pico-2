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

    // printf("Checking if f_mount value = %d", FR_OK);
    // if (FR_OK != fr) {
    //     panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    // }
    // printf("Returned value is valid");

    // ImageReader_drawBMP("pic00.bmp", true, 0, 0, false);

    //ILI9341_demo();

    return 0;
}
