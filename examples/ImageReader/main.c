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

    char * file = "img_00.bmp";
    printf("\nDisplaying \"%s\" properties...\n", file);
    ImageReader_propertiesBMP(file);

    ImageReader_result = ImageReader_drawBMP("img_00.bmp", true, 0, 0, true);

    if (ImageReader_result != IMAGE_SUCCESS)
        panic("ImageReader error: %s (%d)\n", ImageReader_result_str(ImageReader_result), ImageReader_result);
    printf("Image was printed successfully\n");
    
    //ILI9341_demo();

    return 0;
}
