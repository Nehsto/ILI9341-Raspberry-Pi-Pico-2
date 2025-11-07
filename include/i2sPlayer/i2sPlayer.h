#ifndef I2S_PLAYER_I2S_H
#define I2S_PLAYER_I2S_H

#include <string.h>

#include "fileread.h"
#include "f_util.h"
#include "ff.h"

#include "i2s.h"

/** Status codes returned by drawBMP() and loadBMP() */
typedef enum AudioReturnCode {
  AUDIO_SUCCESS,            // Successful load (or image clipped off screen)
  AUDIO_ERR_FILE_NOT_FOUND, // Could not open file
  AUDIO_ERR_FORMAT,         // Not a supported image format
  //IMAGE_ERR_MALLOC          // Could not allocate image (loadBMP() only)
}AudioReturnCode;

AudioReturnCode i2sPlayer_playWAV(const char *filename, bool transact);
void i2sPlayer_propertiesWAV(const char const * filename);

#endif