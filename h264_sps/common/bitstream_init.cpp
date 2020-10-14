#include <string.h>
#include <stdlib.h>
#include "bitstream.h"

struct bitstream_s bitstream;

int bitstream_init(unsigned char *buffer, unsigned int length)
{
	unsigned char *bits;
	unsigned int i;

	if (NULL != bitstream.bits)
		free(bitstream.bits);

	bits = (unsigned char *)malloc(length << 3 + 1);
	if (NULL == bits)
		return -1;

	memset(bits, 0, length << 3);

	for (i = 0; i < (length << 3); i++) {
		bits[i] = (buffer[i >> 3] & (1 << (7 - (i & 0x7)))) ? '1' : '0';
	}

	bitstream.bitpos = 0;
	bitstream.bits = bits;

	return 0;
}
