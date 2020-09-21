#include <stdint.h>
#include <stdio.h>

void hex_dump(uint8_t *buffer, uint32_t length)
{
    uint32_t i, j;
    uint32_t line_num;
    uint32_t last_bytes;

    line_num = length / 16;
    last_bytes = length % 16;

    for (i = 0; i < line_num; i++) {
        printf("%08x+%08x : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
            (uint32_t)&buffer[i * 16], i * 16,
            buffer[i * 16 + 0],
            buffer[i * 16 + 1],
            buffer[i * 16 + 2],
            buffer[i * 16 + 3],
            buffer[i * 16 + 4],
            buffer[i * 16 + 5],
            buffer[i * 16 + 6],
            buffer[i * 16 + 7],
            buffer[i * 16 + 8],
            buffer[i * 16 + 9],
            buffer[i * 16 + 10],
            buffer[i * 16 + 11],
            buffer[i * 16 + 12],
            buffer[i * 16 + 13],
            buffer[i * 16 + 14],
            buffer[i * 16 + 15]);
    }

    if (last_bytes) {
        printf("%08x+%08x : ", (uint32_t)&buffer[i * 16], i * 16);
        for (j = 0; j < length; j++) {
            printf("%02x ", buffer[i * 16 + j]);
        }
        printf("\n");
    }
}
