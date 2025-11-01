#include "ImageReader.h"

#define BUFPIXELS ILI9341_WIDTH

static BYTE read_buffer[4];
static UINT br;

uint8_t read_8bits(FIL *fileptr){
  FRESULT fr = f_read(fileptr, read_buffer, 1, &br);
  if (FR_OK != fr) 
    printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
  return (read_buffer[0]);
}

uint16_t read_16bits(FIL *fileptr){
  FRESULT fr = f_read(fileptr, read_buffer, 2, &br);
  if (FR_OK != fr) 
    printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
  return ((uint16_t)read_buffer[1] << 8 | (uint16_t)read_buffer[0]);
}

uint32_t read_32bits(FIL *fileptr){
  FRESULT fr = f_read(fileptr, read_buffer, 4, &br);
  if (FR_OK != fr) 
    printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
  return ((uint32_t)read_buffer[3] << 24 | (uint32_t)read_buffer[2] << 16 |
    (uint32_t)read_buffer[1] << 8 | (uint32_t)read_buffer[0]);
}

const uint16_t ImageReader_drawBMP(const char *filename,
                                              bool tft, int16_t x,
                                              int16_t y, bool transact) {
  uint16_t tftbuf[BUFPIXELS]; // Temp space for buffering TFT data
  // Call core BMP-reading function, passing address to TFT object,
  // TFT working buffer, and X & Y position of top-left corner (image
  // will be cropped on load if necessary). Image pointer is NULL when
  // reading to TFT, and transact argument is passed through.
  return ImageReader_coreBMP(filename, true, tftbuf, x, y, NULL, transact);
}

const uint16_t ImageReader_coreBMP(
    const char * const filename, // SD file to load
    const bool tft,       // Originally Adafruit_SPITFT* type. Pointer to
                          // TFT object, or NULL if to image
    uint16_t *dest,       // TFT working buffer, or NULL if to canvas
    int16_t x,            // Position if loading to TFT (else ignored)
    int16_t y,
    const int *img,       // Originally Adafruit_Image type, NULL if 
                          // load-to-screen
    bool transact) {      // SD & TFT sharing bus, use transactions

  uint16_t status = IMAGE_ERR_FORMAT; // IMAGE_SUCCESS on valid file
  uint32_t offset;                           // Start of image data in file
  uint32_t headerSize;                       // Indicates BMP version
  int bmpWidth, bmpHeight;                   // BMP width & height in pixels
  uint8_t planes;                            // BMP planes
  uint8_t depth;                             // BMP bit depth
  uint32_t compression = 0;                  // BMP compression mode
  uint32_t colors = 0;                       // Number of colors in palette
  uint16_t *quantized = NULL;                // 16-bit 5/6/5 color palette
  uint32_t rowSize;                          // >bmpWidth if scanline padding
  uint8_t sdbuf[3 * BUFPIXELS];              // BMP read buf (R+G+B/pixel)
#if ((3 * BUFPIXELS) <= 255)
  uint8_t srcidx = sizeof sdbuf; // Current position in sdbuf
#else
  uint16_t srcidx = sizeof sdbuf;
#endif
  uint32_t destidx = 0;
  uint8_t *dest1 = NULL;     // Dest ptr for 1-bit BMPs to img
  bool flip = true;          // BMP is stored bottom-to-top
  uint32_t bmpPos = 0;       // Next pixel position in file
  int loadWidth, loadHeight, // Region being loaded (clipped)
      loadX, loadY;          // "
  int row, col;              // Current pixel pos.
  uint8_t r, g, b;           // Current pixel color
  uint8_t bitIn = 0;         // Bit number for 1-bit data in
  uint8_t bitOut = 0;        // Column mask for 1-bit data out

  //FATFS fs;
  FIL file;
  FRESULT fileError;
  uint32_t init_pos;

  // If an Adafruit_Image object is passed and currently contains anything,
  // free its contents as it's about to be overwritten with new stuff.
  if (img)
    //NOT IMPLEMENTED
    printf("ERROR: SHOULDN'T BE HERE");
    //img->dealloc();

  // If BMP is being drawn off the right or bottom edge of the screen,
  // nothing to do here. NOT an error, just a trivial clip operation.
  if (tft && ((x >= ILI9341_width()) || (y >= ILI9341_height())))
    return IMAGE_SUCCESS;

  // Open requested file on SD card
  if (FR_OK != (fileError = f_open(&file, filename, FA_READ))) {
    return IMAGE_ERR_FILE_NOT_FOUND;
  }

  // Parse BMP header. 0x4D42 (ASCII 'BM') is the Windows BMP signature.
  // There are other values possible in a .BMP file but these are super
  // esoteric (e.g. OS/2 struct bitmap array) and NOT supported here!

  printf("fptr: %d; bmpPos: %d\n\n", (uint32_t)file.fptr, bmpPos);
  uint16_t signature = read_16bits(&file);
  printf("\nSignature: %x\n", signature);
  if (signature == 0x4D42) { // BMP signature
    printf("FILE IS BMP\n");
    read_32bits(&file);          // Read & ignore file size
    read_32bits(&file);          // Read & ignore creator bytes
    offset = read_32bits(&file);      // Start of image data
    // Read DIB header
    headerSize = read_32bits(&file);
    bmpWidth = read_32bits(&file);
    bmpHeight = read_32bits(&file);
    // If bmpHeight is negative, image is in top-down order.
    // This is not canon but has been observed in the wild.
    if (bmpHeight < 0) {
      bmpHeight = -bmpHeight;
      flip = false;
    }
    planes = read_16bits(&file);
    depth = read_16bits(&file); // Bits per pixel
    printf("Depth: %d\n", depth);
    // Compression mode is present in later BMP versions (default = none)
    if (headerSize > 12) {
      compression = read_32bits(&file);
      read_32bits(&file);    // Raw bitmap data size; ignore
      read_32bits(&file);    // Horizontal resolution, ignore
      read_32bits(&file);    // Vertical resolution, ignore
      colors = read_32bits(&file); // Number of colors in palette, or 0 for 2^depth
      read_32bits(&file);    // Number of colors used (ignore)
      // File position should now be at start of palette (if present)
    }
    if (!colors)
      colors = 1 << depth;

    loadWidth = bmpWidth;
    loadHeight = bmpHeight;
    loadX = 0;
    loadY = 0;
    printf("\nTFT: %d\n", tft);
    if (tft) {
      // Crop area to be loaded (if destination is TFT)
      if (x < 0) {
        loadX = -x;
        loadWidth += x;
        x = 0;
      }
      if (y < 0) {
        loadY = -y;
        loadHeight += y;
        y = 0;
      }
      if ((x + loadWidth) > ILI9341_width())
        loadWidth = ILI9341_width() - x;
      if ((y + loadHeight) > ILI9341_height())
        loadHeight = ILI9341_height() - y;
    }
    printf("fptr: %d; bmpPos: %d\n\n", (uint32_t)file.fptr, bmpPos);
    printf("Planes: %d; Compression: %d\n", planes, compression);
    if ((planes == 1) && (compression == 0)) { // Only uncompressed is handled
      printf("Passed!");
      // BMP rows are padded (if needed) to 4-byte boundary
      rowSize = ((depth * bmpWidth + 31) / 32) * 4;

      if ((depth == 24) || (depth == 1)) { // BGR or 1-bit bitmap format

        // if (img) {
          // // Loading to RAM -- allocate GFX 16-bit canvas type
          // status = IMAGE_ERR_MALLOC; // Assume won't fit to start
          // if (depth == 24) {
          //   if ((img->canvas.canvas16 = new GFXcanvas16(bmpWidth, bmpHeight))) {
          //     dest = img->canvas.canvas16->getBuffer();
          //   }
          // } else {
          //   if ((img->canvas.canvas1 = new GFXcanvas1(bmpWidth, bmpHeight))) {
          //     dest1 = img->canvas.canvas1->getBuffer();
          //   }
          // }
          // Future: handle other depths.
          // printf("ERROR: SHOULDN'T BE HERE");
        // }

        printf("\ndest: %x; dest1: %x\n", dest, dest1);
        if (dest || dest1) { // Supported format, alloc OK, etc.
          status = IMAGE_SUCCESS;
          printf("\nProcessing image data...\n");

          if ((loadWidth > 0) && (loadHeight > 0)) { // Clip top/left
            if (tft) {
              ILI9341_startWrite(); // Start SPI (regardless of transact)
              ILI9341_setAddrWindow(x, y, loadWidth, loadHeight);
            } else {
              // if (depth == 1) {
              //   img->format = IMAGE_1; // Is a GFX 1-bit canvas type
              // } else {
              //   img->format = IMAGE_16; // Is a GFX 16-bit canvas type
              // }
              //NOT IMPLEMENTED
              printf("ERROR: SHOULDN'T BE HERE");
            }

            if ((depth >= 16) ||
                (quantized = (uint16_t *)malloc(colors * sizeof(uint16_t)))) {
              if (depth < 16) {
                // Load and quantize color table
                for (uint16_t c = 0; c < colors; c++) {
                  b = read_8bits(&file);
                  g = read_8bits(&file);
                  r = read_8bits(&file);
                  read_8bits(&file); // Ignore 4th byte
                  quantized[c] =
                      ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                }
              }

              for (row = 0; row < loadHeight; row++) { // For each scanline...
                // Seek to start of scan line.  It might seem labor-intensive
                // to be doing this on every line, but this method covers a
                // lot of gritty details like cropping, flip and scanline
                // padding. Also, the seek only takes place if the file
                // position actually needs to change (avoids a lot of cluster
                // math in SD library).
                if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
                  bmpPos = offset + (bmpHeight - 1 - (row + loadY)) * rowSize;
                else // Bitmap is stored top-to-bottom
                  bmpPos = offset + (row + loadY) * rowSize;
                if (depth == 24) {
                  bmpPos += loadX * 3;
                } else {
                  bmpPos += loadX / 8;
                  bitIn = 7 - (loadX & 7);
                  bitOut = 0x80;
                  if (img)
                    destidx = ((bmpWidth + 7) / 8) * row;
                }
                printf("%d == %d? %s\n", (uint32_t)file.fptr, bmpPos, ((uint32_t)file.fptr != bmpPos)? "True" : "False");
                if ((uint32_t)file.fptr != bmpPos) { // Need seek? (Do you need to find where the pointer of the file is?)
                  if (transact) {
                    // ILI9341_dmaWait();
                    ILI9341_endWrite(); // End TFT SPI transaction
                  }
                  f_lseek(&file, bmpPos);     // Seek = SD transaction
                  srcidx = sizeof sdbuf; // Force buffer reload
                }
                printf("AFTER %d == %d? %s\n\n", (uint32_t)file.fptr, bmpPos, ((uint32_t)file.fptr != bmpPos)? "True" : "False");
                printf("loadWidth: %d\n\n", loadWidth);
                printf("destidx: %d\n", destidx);
                for (col = 0; col < loadWidth; col++) { // For each pixel...
                  if (srcidx >= sizeof sdbuf) {         // Time to load more?
                    if (tft) {                          // Drawing to TFT?
                      if (transact) {
                        // ILI9341_dmaWait();
                        ILI9341_endWrite(); // End TFT SPI transact
                      }

                      f_read(&file, sdbuf, sizeof sdbuf, &br); // Load from SD
                      if (transact)
                        ILI9341_startWrite(); // Start TFT SPI transact
                      if (destidx) {       // If buffered TFT data
                        // Non-blocking writes (DMA) have been temporarily
                        // disabled until this can be rewritten with two
                        // alternating 'dest' buffers (else the nonblocking
                        // data out is overwritten in the dest[] write below).
                        // ILI9341_writePixels(dest, destidx, false); // Write it
                        ILI9341_writePixels(dest, destidx); // Write it
                        destidx = 0; // and reset dest index
                      }
                    } else {                          // Canvas is simpler,
                      f_read(&file, sdbuf, sizeof sdbuf, &br); // just load sdbuf
                    } // (destidx never resets)
                    srcidx = 0; // Reset bmp buf index
                  }
                  if (depth == 24) {
                    // Convert each pixel from BMP to 565 format, save in dest
                    b = sdbuf[srcidx++];
                    g = sdbuf[srcidx++];
                    r = sdbuf[srcidx++];
                    dest[destidx++] =
                        //((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                        RGB_to_16bit(r, g, b);
                        printf("destidx: %d\n", destidx);
                  } else {
                    // Extract 1-bit color index
                    uint8_t n = (sdbuf[srcidx] >> bitIn) & 1;
                    if (!bitIn) {
                      srcidx++;
                      bitIn = 7;
                    } else {
                      bitIn--;
                    }
                    if (tft) {
                      // Look up in palette, store in tft dest buf
                      dest[destidx++] = quantized[n];
                    } else {
                      // Store bit in canvas1 buffer (ignore palette)
                      if (n)
                        dest1[destidx] |= bitOut;
                      else
                        dest1[destidx] &= ~bitOut;
                      bitOut >>= 1;
                      if (!bitOut) {
                        bitOut = 0x80;
                        destidx++;
                      }
                    }
                  }
                } // end pixel loop
                printf("col: %d\n", col);
                printf("after, destidx: %d\n", destidx);
                if (tft) {       // Drawing to TFT?
                  if (destidx) { // Any remainders?
                    // See notes above re: DMA
                    // ILI9341_writePixels(dest, destidx, false); // Write it
                    ILI9341_writePixels(dest, destidx); // Write it
                    destidx = 0; // and reset dest index
                  }
                  // ILI9341_dmaWait();
                  ILI9341_endWrite(); // End TFT (regardless of transact)
                }
              } // end scanline loop

              if (quantized) {
                // if (tft)
                  free(quantized); // Palette no longer needed
                // else
                  // img->palette = quantized; // Keep palette with img
              }
            } // end depth>24 or quantized malloc OK
          } // end top/left clip
        } // end malloc check
      } // end depth check
    } // end planes/compression check
  } // end signature

  fileError = f_close(&file);
  if (FR_OK != fileError) {
    printf("f_close error: %s (%d)\n", FRESULT_str(fileError), fileError);
  }
  return status;
}

void ImageReader_propertiesBMP(const char * const filename){

  FIL file;
  FRESULT fileError;

  if (FR_OK != (fileError = f_open(&file, filename, FA_READ))) {
    printf("properties_FileError: \"%s\" not found\n", filename);
    return;
  }

  printf("signature: %x\n", read_16bits(&file));
  read_32bits(&file);
  read_32bits(&file);
  printf("offset: %d\n", read_32bits(&file));
  uint32_t headerSize = read_32bits(&file);
  printf("headerSize: %d\n", headerSize);
  printf("bmpWidth: %d\n", read_32bits(&file));
  printf("bmpHeight: %d\n", read_32bits(&file));
  printf("planes: %d\n", read_16bits(&file));
  printf("depth: %d\n", read_16bits(&file));
  if (headerSize > 12) {
      printf("compression: %d\n",read_32bits(&file));
      read_32bits(&file);    // Raw bitmap data size; ignore
      read_32bits(&file);    // Horizontal resolution, ignore
      read_32bits(&file);    // Vertical resolution, ignore
      printf("colors: %d\n", read_32bits(&file)); // Number of colors in palette, or 0 for 2^depth
      read_32bits(&file);    // Number of colors used (ignore)
      // File position should now be at start of palette (if present)
    }

  fileError = f_close(&file);
  if (FR_OK != fileError) {
    printf("f_close error: %s (%d)\n", FRESULT_str(fileError), fileError);
  }
}

const char * const ImageReader_result_str(ImageReturnCode code){
  switch(code){
    case IMAGE_SUCCESS:            // Successful load (or image clipped off screen)
      return "IMAGE_SUCCESS";
      break;
    case IMAGE_ERR_FILE_NOT_FOUND: // Could not open file
      return "IMAGE_ERR_FILE_NOT_FOUND";
      break;
    case IMAGE_ERR_FORMAT:         // Not a supported image format
      return "IMAGE_ERR_FORMAT";
      break;
    case IMAGE_ERR_MALLOC: 
      return "IMAGE_ERR_MALLOC";
      break;
    default:
      return "UNKNOWN_ERROR";
      break;
  }
}