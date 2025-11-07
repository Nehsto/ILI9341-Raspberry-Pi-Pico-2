#ifndef IMAGE_READER_H
#define IMAGE_READER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "ILI9341.h"
#include "fileread.h"
#include "f_util.h"
#include "ff.h"

/** Status codes returned by drawBMP() and loadBMP() */
typedef enum ImageReturnCode {
  IMAGE_SUCCESS,            // Successful load (or image clipped off screen)
  IMAGE_ERR_FILE_NOT_FOUND, // Could not open file
  IMAGE_ERR_FORMAT,         // Not a supported image format
  IMAGE_ERR_MALLOC          // Could not allocate image (loadBMP() only)
}ImageReturnCode;

/** Image formats returned by loadBMP() */
typedef enum ImageFormat {
  IMAGE_NONE, // No image was loaded; IMAGE_ERR_* condition
  IMAGE_1,    // GFXcanvas1 image (NOT YET SUPPORTED)
  IMAGE_8,    // GFXcanvas8 image (NOT YET SUPPORTED)
  IMAGE_16    // GFXcanvas16 image (SUPPORTED)
}ImageFormat;

const uint16_t ImageReader_drawBMP(const char * const filename, const bool tft, int16_t x, //Adafruit_SPITFT &tft, 
                          int16_t y, bool transact);
const uint16_t ImageReader_coreBMP(const char * const filename, const bool tft,
                          uint16_t *dest, int16_t x, int16_t y,
                          const int *img, bool transact);\
void ImageReader_propertiesBMP(const char * const filename);
const char * const ImageReader_result_str(ImageReturnCode code);

#endif 