#include <stdio.h>

#include <pico/stdlib.h>

#include "ILI9341.h"
#include "ImageReader.h"

int main(){
    stdio_init_all();
    sleep_ms(1000);
    
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
    
    /* DEBUG
    char *files[3] = {"img_00.bmp", "gimp_options/img_00_gimp.bmp", "24bit/img_00.bmp"};
    for(int i = 0; i < 3; i++){
        printf("\nDisplaying \"%s\" properties...\n", files[i]);
        ImageReader_propertiesBMP(files[i]);
    }
    */

    ImageReader_result = ImageReader_drawBMP("img_00.bmp", true, 0, 0, true);
    if (ImageReader_result != IMAGE_SUCCESS)
        panic("ImageReader error: %s (%d)\n", ImageReader_result_str(ImageReader_result), ImageReader_result);
    printf("Image in file %s was printed successfully\n", "img_00.bmp");

    // char *files[5] = {"img_00.bmp", "img_01.bmp", "img_02.bmp", "img_03.bmp", "img_04.bmp"};
    // for(int i = 0; i < 5; i++){
    //     ImageReader_result = ImageReader_drawBMP(files[i], true, 0, 0, true);

    //     if (ImageReader_result != IMAGE_SUCCESS)
    //         panic("ImageReader error: %s (%d)\n", ImageReader_result_str(ImageReader_result), ImageReader_result);
    //     printf("Image in file %s was printed successfully\n", files[i]);
    // }

    return 0;
}
