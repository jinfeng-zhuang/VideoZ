#include "main.h"

unsigned int ue(void)
{
	unsigned int count = 0;

	while (0 == u(1))
		count++;

	bitstream.bitpos--;

	return u(count + 1) - 1;
}
