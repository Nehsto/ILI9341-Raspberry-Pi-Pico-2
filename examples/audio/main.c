#include <stdio.h>
#include <pico/stdlib.h>

#include "ImageReader.h"
#include "i2sPlayer.h"

int main(){
    stdio_init_all();
    sleep_ms(5000);
    
    FATFS fs;
    FRESULT fr;

    printf("Mounting SD card...\n");
    fr = f_mount(&fs, "", 1);
    printf("f_mount returned: %d\n", fr);

    printf("Checking if f_mount value = %d\n", FR_OK);
    if (FR_OK != fr) {
        panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    }
    printf("Returned value is valid\n");

    i2sPlayer_propertiesWAV("music_box.wav");

    return 0;
}
