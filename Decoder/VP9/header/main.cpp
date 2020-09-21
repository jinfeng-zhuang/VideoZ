#include <Windows.h>
#include <stdint.h>
#include <stdio.h>

#include "nalu.h"

//=============================================================================
// External Symbols
//=============================================================================

extern unsigned char *file_load(const char *filename, unsigned int *length);

extern void hex_dump(uint8_t *buffer, uint32_t length);

extern int nalu_init(unsigned char *buffer, unsigned int length);
extern int nalu_uninit(void);
extern int nalu_read(unsigned int index, struct nalu *nalu);

//=============================================================================
// Types
//=============================================================================

enum {
    SEI_PIC_TIMING = 1,
};

struct ivf_file_header {
    unsigned int signature;
    unsigned short version;
    unsigned short length;
    unsigned int codec;
    unsigned short width;
    unsigned short height;
    unsigned int framerate;
    unsigned int timescale;
    unsigned int framenum;
    unsigned int unused;
};

//=============================================================================
// Local Functions
//=============================================================================

//=============================================================================
// Global Functions
//=============================================================================

int main(int argc, char *argv[])
{
    unsigned int file_size;
    unsigned char *file_content;
    unsigned int prev_offset = 0;
    unsigned int prev_size = 0;

    struct ivf_file_header *file_header;

    unsigned int length;
    unsigned long long timestamp;

    unsigned int i;

    file_content = file_load("dump-000.es", &file_size);
    if (NULL == file_content)
        return -1;

    file_header = (struct ivf_file_header *)file_content;

    for (i = 0x20; i < file_size;) {
        length = *((unsigned int *)&file_content[i]);
        timestamp = *((unsigned long long *)&file_content[i + 4]);
        //printf("%08x: size %d: %08llx\n", i, length, timestamp);
		printf("size %08x, offset %08x, expected %08x\n", length, i, prev_offset + prev_size + 12);
        prev_offset = i;
        prev_size = length;
        i += 12;
        i += length;
    }

    free(file_content);

    return 0;
}