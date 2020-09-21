#include "bitstream.h"

int se(void)
{
	unsigned int count = 0;
	unsigned int value;
	unsigned int symbol;

	while (0 == u(1))
		count++;

	bitstream.bitpos--;

	value = u(count);
	symbol = u(1) ? -1 : 1;

	return value * symbol;
}
