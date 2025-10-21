#ifndef IMAGE_READER_H
#define IMAGE_READER_H

<<<<<<< HEAD
=======
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "ILI9341.h"
#include "f_util.h"
#include "ff.h"

>>>>>>> main
/** Status codes returned by drawBMP() and loadBMP() */
enum ImageReturnCode {
  IMAGE_SUCCESS,            // Successful load (or image clipped off screen)
  IMAGE_ERR_FILE_NOT_FOUND, // Could not open file
  IMAGE_ERR_FORMAT,         // Not a supported image format
  IMAGE_ERR_MALLOC          // Could not allocate image (loadBMP() only)
};

/** Image formats returned by loadBMP() */
enum ImageFormat {
  IMAGE_NONE, // No image was loaded; IMAGE_ERR_* condition
  IMAGE_1,    // GFXcanvas1 image (NOT YET SUPPORTED)
  IMAGE_8,    // GFXcanvas8 image (NOT YET SUPPORTED)
  IMAGE_16    // GFXcanvas16 image (SUPPORTED)
};

<<<<<<< HEAD
const int ImageReader_drawBMP(const char *filename, Adafruit_SPITFT &tft, int16_t x,
                          int16_t y, boolean transact = true);
const int ImageReader_coreBMP(const char *filename, Adafruit_SPITFT *tft,
                          uint16_t *dest, int16_t x, int16_t y,
                          Adafruit_Image *img, boolean transact);
=======
const int ImageReader_drawBMP(const char * const filename, const bool tft, int16_t x, //Adafruit_SPITFT &tft, 
                          int16_t y, bool transact = true);
const int ImageReader_coreBMP(const char *filename, Adafruit_SPITFT *tft,
                          uint16_t *dest, int16_t x, int16_t y,
                          Adafruit_Image *img, bool transact);
>>>>>>> main

#endif 