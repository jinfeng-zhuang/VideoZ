#ifndef JPEG_SECTION_H
#define JPEG_SECTION_h

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <tchar.h>

#include "jpeg.h"
#include "bmp.h"

#include "jpeg_decoder.h"

#include "config.h"

#define BIGENDIAN_16(x) (((x>>8)&0xFF) | ((x&0xFF)<<8))

extern int output(uint8_t *matrix);
extern int load_file(const char *filename, uint8_t **buffer, uint32_t *length);

extern struct huffman_db huffman_table;
extern uint8_t zigzag_array_reverse[];

#endif