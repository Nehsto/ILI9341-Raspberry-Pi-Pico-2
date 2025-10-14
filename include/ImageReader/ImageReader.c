#include "ImageReader.h"

const int ImageReader_coreBMP(
    const char *filename, // SD file to load
    const int *tft, // Pointer to TFT object, or NULL if to image
    uint16_t *dest,       // TFT working buffer, or NULL if to canvas
    int16_t x,            // Position if loading to TFT (else ignored)
    int16_t y,
    Adafruit_Image *img, // NULL if load-to-screen
    boolean transact) {  // SD & TFT sharing bus, use transactions

  ImageReturnCode status = IMAGE_ERR_FORMAT; // IMAGE_SUCCESS on valid file
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
  boolean flip = true;       // BMP is stored bottom-to-top
  uint32_t bmpPos = 0;       // Next pixel position in file
  int loadWidth, loadHeight, // Region being loaded (clipped)
      loadX, loadY;          // "
  int row, col;              // Current pixel pos.
  uint8_t r, g, b;           // Current pixel color
  uint8_t bitIn = 0;         // Bit number for 1-bit data in
  uint8_t bitOut = 0;        // Column mask for 1-bit data out

  // If an Adafruit_Image object is passed and currently contains anything,
  // free its contents as it's about to be overwritten with new stuff.
  if (img)
    img->dealloc();

  // If BMP is being drawn off the right or bottom edge of the screen,
  // nothing to do here. NOT an error, just a trivial clip operation.
  if (tft && ((x >= tft->width()) || (y >= tft->height())))
    return IMAGE_SUCCESS;

  // Open requested file on SD card
  if (!(file = filesys->open(filename, FILE_READ))) {
    return IMAGE_ERR_FILE_NOT_FOUND;
  }

  // Parse BMP header. 0x4D42 (ASCII 'BM') is the Windows BMP signature.
  // There are other values possible in a .BMP file but these are super
  // esoteric (e.g. OS/2 struct bitmap array) and NOT supported here!
  if (readLE16() == 0x4D42) { // BMP signature
    (void)readLE32();         // Read & ignore file size
    (void)readLE32();         // Read & ignore creator bytes
    offset = readLE32();      // Start of image data
    // Read DIB header
    headerSize = readLE32();
    bmpWidth = readLE32();
    bmpHeight = readLE32();
    // If bmpHeight is negative, image is in top-down order.
    // This is not canon but has been observed in the wild.
    if (bmpHeight < 0) {
      bmpHeight = -bmpHeight;
      flip = false;
    }
    planes = readLE16();
    depth = readLE16(); // Bits per pixel
    // Compression mode is present in later BMP versions (default = none)
    if (headerSize > 12) {
      compression = readLE32();
      (void)readLE32();    // Raw bitmap data size; ignore
      (void)readLE32();    // Horizontal resolution, ignore
      (void)readLE32();    // Vertical resolution, ignore
      colors = readLE32(); // Number of colors in palette, or 0 for 2^depth
      (void)readLE32();    // Number of colors used (ignore)
      // File position should now be at start of palette (if present)
    }
    if (!colors)
      colors = 1 << depth;

    loadWidth = bmpWidth;
    loadHeight = bmpHeight;
    loadX = 0;
    loadY = 0;
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
      if ((x + loadWidth) > tft->width())
        loadWidth = tft->width() - x;
      if ((y + loadHeight) > tft->height())
        loadHeight = tft->height() - y;
    }

    if ((planes == 1) && (compression == 0)) { // Only uncompressed is handled

      // BMP rows are padded (if needed) to 4-byte boundary
      rowSize = ((depth * bmpWidth + 31) / 32) * 4;

      if ((depth == 24) || (depth == 1)) { // BGR or 1-bit bitmap format

        if (img) {
          // Loading to RAM -- allocate GFX 16-bit canvas type
          status = IMAGE_ERR_MALLOC; // Assume won't fit to start
          if (depth == 24) {
            if ((img->canvas.canvas16 = new GFXcanvas16(bmpWidth, bmpHeight))) {
              dest = img->canvas.canvas16->getBuffer();
            }
          } else {
            if ((img->canvas.canvas1 = new GFXcanvas1(bmpWidth, bmpHeight))) {
              dest1 = img->canvas.canvas1->getBuffer();
            }
          }
          // Future: handle other depths.
        }

        if (dest || dest1) { // Supported format, alloc OK, etc.
          status = IMAGE_SUCCESS;

          if ((loadWidth > 0) && (loadHeight > 0)) { // Clip top/left
            if (tft) {
              tft->startWrite(); // Start SPI (regardless of transact)
              tft->setAddrWindow(x, y, loadWidth, loadHeight);
            } else {
              if (depth == 1) {
                img->format = IMAGE_1; // Is a GFX 1-bit canvas type
              } else {
                img->format = IMAGE_16; // Is a GFX 16-bit canvas type
              }
            }

            if ((depth >= 16) ||
                (quantized = (uint16_t *)malloc(colors * sizeof(uint16_t)))) {
              if (depth < 16) {
                // Load and quantize color table
                for (uint16_t c = 0; c < colors; c++) {
                  b = file.read();
                  g = file.read();
                  r = file.read();
                  (void)file.read(); // Ignore 4th byte
                  quantized[c] =
                      ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
                }
              }

              for (row = 0; row < loadHeight; row++) { // For each scanline...
#ifdef ESP8266
                delay(1); // Keep ESP8266 happy
#endif
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
                if (file.position() != bmpPos) { // Need seek?
                  if (transact) {
                    tft->dmaWait();
                    tft->endWrite(); // End TFT SPI transaction
                  }
                  file.seek(bmpPos);     // Seek = SD transaction
                  srcidx = sizeof sdbuf; // Force buffer reload
                }
                for (col = 0; col < loadWidth; col++) { // For each pixel...
                  if (srcidx >= sizeof sdbuf) {         // Time to load more?
                    if (tft) {                          // Drawing to TFT?
                      if (transact) {
                        tft->dmaWait();
                        tft->endWrite(); // End TFT SPI transact
                      }
#if defined(ARDUINO_NRF52_ADAFRUIT)
                      // NRF52840 seems to have trouble reading more than 512
                      // bytes across certain boundaries. Workaround for now
                      // is to break the read into smaller chunks...
                      int32_t bytesToGo = sizeof sdbuf, bytesRead = 0,
                              bytesThisPass;
                      while (bytesToGo > 0) {
                        bytesThisPass = min(bytesToGo, 512);
                        file.read(&sdbuf[bytesRead], bytesThisPass);
                        bytesRead += bytesThisPass;
                        bytesToGo -= bytesThisPass;
                      }
#else
                      file.read(sdbuf, sizeof sdbuf); // Load from SD
#endif
                      if (transact)
                        tft->startWrite(); // Start TFT SPI transact
                      if (destidx) {       // If buffered TFT data
                        // Non-blocking writes (DMA) have been temporarily
                        // disabled until this can be rewritten with two
                        // alternating 'dest' buffers (else the nonblocking
                        // data out is overwritten in the dest[] write below).
                        // tft->writePixels(dest, destidx, false); // Write it
                        tft->writePixels(dest, destidx, true); // Write it
                        destidx = 0; // and reset dest index
                      }
                    } else {                          // Canvas is simpler,
                      file.read(sdbuf, sizeof sdbuf); // just load sdbuf
                    } // (destidx never resets)
                    srcidx = 0; // Reset bmp buf index
                  }
                  if (depth == 24) {
                    // Convert each pixel from BMP to 565 format, save in dest
                    b = sdbuf[srcidx++];
                    g = sdbuf[srcidx++];
                    r = sdbuf[srcidx++];
                    dest[destidx++] =
                        ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
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
                if (tft) {       // Drawing to TFT?
                  if (destidx) { // Any remainders?
                    // See notes above re: DMA
                    // tft->writePixels(dest, destidx, false); // Write it
                    tft->writePixels(dest, destidx, true); // Write it
                    destidx = 0; // and reset dest index
                  }
                  tft->dmaWait();
                  tft->endWrite(); // End TFT (regardless of transact)
                }
              } // end scanline loop

              if (quantized) {
                if (tft)
                  free(quantized); // Palette no longer needed
                else
                  img->palette = quantized; // Keep palette with img
              }
            } // end depth>24 or quantized malloc OK
          } // end top/left clip
        } // end malloc check
      } // end depth check
    } // end planes/compression check
  } // end signature

  file.close();
  return status;
}