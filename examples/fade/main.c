#include <stdio.h>

#include <pico/stdlib.h>

#include "ILI9341.h"
#include "ImageReader.h"

int main(){
    stdio_init_all();
    sleep_ms(5000);
    
    ILI9341_init(HORIZONTAL);

    FATFS fs;
    FRESULT fr;

    ImageReturnCode ImageReader_result;

    printf("Mounting SD card...\n");
    fr = f_mount(&fs, "", 1);
    printf("f_mount returned: %d\n", fr);

    printf("Checking if f_mount value = %d\n", FR_OK);
    if (FR_OK != fr) {
        panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    }
    printf("Returned value is valid\n");

    char *files[5] = {"img_00.bmp", "img_01.bmp", "img_02.bmp", "img_03.bmp", "img_04.bmp"};
    for(int i = 0; i < 5; i++){
        ILI9341_toggle_led(false);
        while(ILI9341_is_led_switching())
            tight_loop_contents();

        ImageReader_result = ImageReader_drawBMP(files[i], true, 0, 0, true);
        if (ImageReader_result != IMAGE_SUCCESS)
            panic("ImageReader error: %s (%d)\n", ImageReader_result_str(ImageReader_result), ImageReader_result);
        printf("Image in file %s was printed successfully\n", files[i]);

        ILI9341_toggle_led(true);
        sleep_ms(DELAYMS_HOLD_IMAGE);
    }

    return 0;
}
