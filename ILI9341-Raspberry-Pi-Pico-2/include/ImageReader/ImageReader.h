#ifndef IMAGE_READER_H
#define IMAGE_READER_H

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

const int ImageReader_drawBMP(const char *filename, Adafruit_SPITFT &tft, int16_t x,
                          int16_t y, boolean transact = true);
const int ImageReader_coreBMP(const char *filename, Adafruit_SPITFT *tft,
                          uint16_t *dest, int16_t x, int16_t y,
                          Adafruit_Image *img, boolean transact);

#endif 