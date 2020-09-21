#include "main.h"

static char debug_str[33];
static int error;

unsigned int u(unsigned int num)
{
	unsigned int i;
	unsigned int value;

	value = 0;

	/* check if overflow */
	if (bitstream.bitpos >= strlen((const char*)bitstream.bits)) {
		error = -1;
		return 0; /* return 0 even if failed */
	}
	else {
		error = 0;
	}

	memcpy(debug_str, &bitstream.bits[bitstream.bitpos], 32);
	debug_str[32] = 0;
	//printf("%s\n", debug_str);
	
	for (i = 0; i < num; i++) {
		value = value << 1;
		value |= bitstream.bits[bitstream.bitpos] == '1'  ? 1 : 0;
		bitstream.bitpos++;
	}

	return value;
}

int bitstream_error(void)
{
	return (error ? 1 : 0);
}
