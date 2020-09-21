#include "main.h"

int more_rbsp_data(void)
{
	if (bitstream.bitpos >= strlen((const char*)bitstream.bits))
		return 0;

	return 1;
}
