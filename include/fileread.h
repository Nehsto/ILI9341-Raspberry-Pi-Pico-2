#ifndef FILEREAD_H
#define FILEREAD_H

#include <stdio.h>

#include "f_util.h"
#include "ff.h"

static BYTE rbuf[4];
static UINT br;

uint8_t read_8bits(FIL *fileptr);
uint16_t read_16bits(FIL *fileptr);
uint32_t read_32bits(FIL *fileptr);
void read_buffer(FIL *fileptr, char* buff, uint32_t len);

#endif