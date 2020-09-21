#include "main.h"

int byte_aligned(void)
{
    return (0 == (bitstream.bitpos & 0x7));
}
