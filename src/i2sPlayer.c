#include "i2sPlayer.h"
#include "i2s.h"
#include "hardware/dma.h"

AudioReturnCode i2sPlayer_playWAV(const char const *filename, pio_i2s *i2s){
  char buff[5];                 // fileMark: Marks the file as a "RIFF" file. Characters are each 1 byte long.
  buff[4] = '\0';
  uint32_t fileSize;            // Size of the overall file
  // fileType                   // File Type Header. For our purposes, it always equals “WAVE”.
  // format                     // Format chunk marker "fmt ", including trail null
  uint32_t blockSize;           // Length of format data as listed above
  uint16_t audioFormat;         // Type of format (1 is PCM) - 2 byte integer
  uint16_t numChannels;         // Number of Channels - 2 byte integer
  uint32_t sampleRate;          // Number of Samples per second, or Hertz
  uint32_t bytepSec;            // (Sample Rate * BitsPerSample * Channels) / 8
  uint16_t bytepBlock;          // (BitsPerSample * Channels) / 8.1 - 8 bit mono2 - 8 bit stereo/16 bit mono4 - 16 bit stereo
  uint16_t bitpSample;          // Bits per sample
  // dataChunk                  // Marks the beginning of the data frame
  uint32_t dataSize;            // Size of the data section in bytes.

  AudioReturnCode status = AUDIO_ERR_FORMAT;

  FIL file;
  FRESULT fileError;

  // Open requested file on SD card
  if (FR_OK != (fileError = f_open(&file, filename, FA_READ))) {
    return AUDIO_ERR_FILE_NOT_FOUND;
  }

  read_buffer(&file, buff, 4);
  if(strcmp(buff, "RIFF") != 0)
    return AUDIO_ERR_FORMAT;

  fileSize = read_32bits(&file);

  read_buffer(&file, buff, 4);
  if(strcmp(buff, "WAVE") != 0)
    return AUDIO_ERR_FORMAT;

  read_buffer(&file, buff, 4);
  if(strcmp(buff, "fmt ") != 0)
    return AUDIO_ERR_FORMAT;

  blockSize = read_32bits(&file);
  audioFormat = read_16bits(&file);
  numChannels = read_16bits(&file);
  sampleRate = read_32bits(&file);
  bytepSec = read_32bits(&file);
  bytepBlock = read_16bits(&file);
  bitpSample = read_16bits(&file);

  if (strfind(&file, "data")){
    printf("Reading data...\n");
  }
  else
    panic("\"data\" string was not found in file. Aborting...");

  dataSize = read_32bits(&file);

  // // Wait for first DMA IRQ to know which buffer DMA is currently using
  // printf("Waiting for first DMA IRQ to determine buffer state...\n");
  // while(!write_to){
  //   printf("Waiting...\n");
  // }
  // write_to = false;

  // Determine which output buffer is free (the one DMA is NOT currently using)
  // Get the actual buffer pointer that DMA control channel is pointing to
  // int32_t* dma_current_buffer = *(int32_t**)dma_hw->ch[i2s->dma_ch_out_ctrl].read_addr;
  
  // printf("DEBUG: DMA control is reading buffer pointer: %p\n", (void*)dma_current_buffer);
  // printf("DEBUG: Buffer 0 address: %p\n", (void*)i2s->output_buffer);
  // printf("DEBUG: Buffer 1 address: %p\n", (void*)&i2s->output_buffer[STEREO_BUFFER_SIZE]);
  
  
  // if (dma_current_buffer == i2s->output_buffer) {
  //   // DMA is reading from first buffer, so we can write to second buffer
  //   target_buffer = &i2s->output_buffer[STEREO_BUFFER_SIZE];
  //   printf("DEBUG: DMA using buffer 0, writing to buffer 1\n");
  // } else {
  //   // DMA is reading from second buffer, so we can write to first buffer
  //   target_buffer = i2s->output_buffer;
  //   printf("DEBUG: DMA using buffer 1, writing to buffer 0\n");
  // }

  // Pre-fill first buffer - read WAV data directly into output buffer (16-bit, no conversion needed)
  //read_buffer(&file, temp, STEREO_BUFFER_SIZE * sizeof(uint16_t));
  // printf("First buffer filled at %p. First 4 bytes: %02x %02x %02x %02x\n",
  //       temp,
  //       ((uint8_t*)temp)[0],
  //       ((uint8_t*)temp)[1], 
  //       ((uint8_t*)temp)[2],
  //       ((uint8_t*)temp)[3]);
  // while(!write_to){
  //   printf("Waiting...\n");
  // }
  // write_to = false;

  // // Pre-fill second buffer
  // target_buffer = (target_buffer == i2s->output_buffer) ? &i2s->output_buffer[STEREO_BUFFER_SIZE] : i2s->output_buffer;
  // read_buffer(&file, (uint8_t*)target_buffer, STEREO_BUFFER_SIZE * sizeof(uint16_t));
  // printf("Second buffer filled at %p. First 4 bytes: %02x %02x %02x %02x\n",
  //       target_buffer,
  //       ((uint8_t*)target_buffer)[0],
  //       ((uint8_t*)target_buffer)[1], 
  //       ((uint8_t*)target_buffer)[2],
  //       ((uint8_t*)target_buffer)[3]);
  // write_to = false;

  // Calculate number of buffer chunks needed
  // dataSize is in bytes, each buffer needs STEREO_BUFFER_SIZE * 2 bytes (16-bit samples)
  uint32_t num_chunks = dataSize / (STEREO_BUFFER_SIZE * sizeof(uint16_t));
  uint32_t t;
  int32_t temp[STEREO_BUFFER_SIZE];

  int32_t* target_buffer;
  int32_t* dma_current_buffer = NULL;
  int32_t* last_buffer = NULL;
  // Feed remaining data as buffers are consumed
  for(uint32_t i = 0; i < num_chunks; i++){
    read_buffer(&file, temp, STEREO_BUFFER_SIZE * sizeof(uint16_t));
    
    // for(int i = 0; i < STEREO_BUFFER_SIZE; i++){
    //   t = temp[i] & 0xFF;
    //   temp[i] = (temp[i] >> 8) | (t << 8);
    // }

    // Determine which buffer is free
    // Get the actual buffer pointer that DMA control channel is pointing to
    
    while(last_buffer == dma_current_buffer){
      dma_current_buffer = *(int32_t**)dma_hw->ch[i2s->dma_ch_out_ctrl].read_addr;
    }

    if (dma_current_buffer == i2s->output_buffer) {
      // DMA is reading from first buffer, so we can write to second buffer
      target_buffer = &i2s->output_buffer[STEREO_BUFFER_SIZE];
      printf("First buffer\n");
    } else {
      // DMA is reading from second buffer, so we can write to first buffer
      target_buffer = i2s->output_buffer;
      printf("Second buffer\n");
    }

    // Transfer chunk to output buffer - no conversion needed since both are 16-bit
    for(int i = 0; i < STEREO_BUFFER_SIZE; i++){
      target_buffer[i] = temp[i];
    }

    last_buffer = dma_current_buffer;

    // while(!write_to) {printf("Waiting...\n");}; // Wait for DMA to finish current buffer
    // write_to = false;
  }

  status = AUDIO_SUCCESS; 

  fileError = f_close(&file);
  if (FR_OK != fileError) {
    printf("f_close error: %s (%d)\n", FRESULT_str(fileError), fileError);
  }
  return status;
}

void i2sPlayer_propertiesWAV(const char const * filename){
  char buff[5];
  buff[4] = '\0';

  FIL file;
  FRESULT fileError;

  int dataSize; 
  int bytepBlock;

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
  printf("bytepBlock: %d\n", (bytepBlock = read_16bits(&file)));
  printf("bitpSample: %d\n", read_16bits(&file));

  if (strfind(&file, "data")){
    printf("Reading data...\n");
  }
  else
    panic("\"data\" string was not found in file. Aborting...");

  printf("dataSize: %d\n", (dataSize = read_32bits(&file))); 

  for(int i = 0; i < (dataSize / bytepBlock); i++){
    printf("Left sample: %x Right sample: %x\n", read_16bits(&file), read_16bits(&file));
    sleep_ms(10);
  }

  fileError = f_close(&file);
  if (FR_OK != fileError) {
    printf("f_close error: %s (%d)\n", FRESULT_str(fileError), fileError);
  }
}