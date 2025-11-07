#include "i2sPlayer.h"

AudioReturnCode i2sPlayer_playWAV(const char *filename, bool transact){
  char fileMark[5];             // Marks the file as a "RIFF" file. Characters are each 1 byte long.
  fileMark[4] = '\0';
  uint32_t fileSize;            // Size of the overall file
  char fileType[5];             // File Type Header. For our purposes, it always equals “WAVE”.
  fileType[4] = '\0';
  char format[5];               // Format chunk marker "fmt ", including trail null
  format[4] =  '\0';
  uint32_t blockSize;           // Length of format data as listed above
  uint16_t audioFormat;         // Type of format (1 is PCM) - 2 byte integer
  uint16_t numChannels;         // Number of Channels - 2 byte integer
  uint32_t sampleRate;          // Number of Samples per second, or Hertz
  uint32_t bytepSec;            // (Sample Rate * BitsPerSample * Channels) / 8
  uint16_t bytepBlock;          // (BitsPerSample * Channels) / 8.1 - 8 bit mono2 - 8 bit stereo/16 bit mono4 - 16 bit stereo
  uint16_t bitpSample;          // Bits per sample
  char dataChunk[5];            // Marks the beginning of the data frame
  dataChunk[4] = '\0';
  uint32_t dataSize;            // Size of the data section.
  BYTE data[AUDIO_BUFFER_FRAMES];

  AudioReturnCode status = AUDIO_ERR_FORMAT;

  FIL file;
  FRESULT fileError;
  uint32_t init_pos;

  // Open requested file on SD card
  if (FR_OK != (fileError = f_open(&file, filename, FA_READ))) {
    return AUDIO_ERR_FILE_NOT_FOUND;
  }

  read_buffer(&file, fileMark, 4);
  if(strcmp(fileMark, "RIFF") != 0)
    return AUDIO_ERR_FORMAT;

  fileSize = read_32bits(&file);

  read_buffer(&file, fileType, 4);
  if(strcmp(fileType, "WAVE") != 0)
    return AUDIO_ERR_FORMAT;

  read_buffer(&file, format, 4);
  if(strcmp(format, "fmt ") != 0)
    return AUDIO_ERR_FORMAT;

  blockSize = read_32bits(&file);
  audioFormat = read_16bits(&file);
  numChannels = read_16bits(&file);
  sampleRate = read_32bits(&file);
  bytepSec = read_32bits(&file);
  bytepBlock = read_16bits(&file);
  bitpSample = read_16bits(&file);

  read_buffer(&file, dataChunk, 4);
  if(strcmp(dataChunk, "data") != 0)
    return AUDIO_ERR_FORMAT;

  dataSize = read_32bits(&file);

  return status;
}


void i2sPlayer_propertiesWAV(const char const * filename){
  char buff[5];
  buff[4] = '\0';

  FIL file;
  FRESULT fileError;
  uint32_t init_pos;

  // Open requested file on SD card
  if (FR_OK != (fileError = f_open(&file, filename, FA_READ))) {
    panic("AUDIO_ERR_FILE_NOT_FOUND");
  }
  
  read_buffer(&file, buff, 4);
  printf("fileMark: %s\n", buff);
  printf("fileSize: %u\n", read_32bits(&file));
  read_buffer(&file, buff, 4);
  printf("fileType: %s\n", buff);
  read_buffer(&file, buff, 4);
  printf("format: %s\n", buff);
  printf("blockSize: %d\n", read_32bits(&file));
  printf("audioFormat: %d\n", read_16bits(&file));
  printf("numChannels: %d\n", read_16bits(&file));
  printf("sampleRate: %u\n", read_32bits(&file));
  printf("bytepSec: %u\n", read_32bits(&file));
  printf("bytepBlock: %d\n", read_16bits(&file));
  printf("bitpSample: %d\n", read_16bits(&file));
  read_buffer(&file, buff, 4);
  printf("dataChunk: %s\n", buff);
  printf("dataSize: %d\n", read_32bits(&file));

  while (strcmp(buff, "data") != 0){
    read_buffer(&file, buff, 4);
    printf("%s (0x%x)\n", buff, buff);
    sleep_ms(1000);
  }
}