#include "fileread.h"

uint8_t read_8bits(FIL *fileptr){
  FRESULT fr = f_read(fileptr, rbuf, 1, &br);
  if (FR_OK != fr) 
    printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
  return (rbuf[0]);
}

uint16_t read_16bits(FIL *fileptr){
  FRESULT fr = f_read(fileptr, rbuf, 2, &br);
  if (FR_OK != fr) 
    printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
  return ((uint16_t)rbuf[1] << 8 | (uint16_t)rbuf[0]);
}

uint32_t read_32bits(FIL *fileptr){
  FRESULT fr = f_read(fileptr, rbuf, 4, &br);
  if (FR_OK != fr) 
    printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
  return ((uint32_t)rbuf[3] << 24 | (uint32_t)rbuf[2] << 16 |
    (uint32_t)rbuf[1] << 8 | (uint32_t)rbuf[0]);
}

void read_buffer(FIL *fileptr, void* buff, uint32_t len){
  FRESULT fr = f_read(fileptr, buff, len, &br);
  if (FR_OK != fr) 
      printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
}

// Find the first iteration of the string 'str' in 'file' 
// without changing the initial file position
bool strfind(FIL *file, const char const * str){
  int i = 0;
  int strSize = 0;
  char c;

  for(strSize = 0; str[strSize] != '\0'; strSize++);
  //printf("strSize: %d", strSize);
  for(i = 0; i < strSize && (c = read_8bits(file)) != EOF;)
   (c == str[i])? i++ : (i = 0);

  return i == strSize;
}