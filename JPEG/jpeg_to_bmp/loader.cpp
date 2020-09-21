/*
 * Attention
 *		1. can't apply to file size > 2.1G
 */

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

int load_file(const char *filename, uint8_t **buffer, uint32_t *length)
{
	assert(NULL != filename);
	assert(NULL == *buffer);
	assert((NULL != length) && (0 == *length));

	FILE *f = NULL;
	int ret;
	long filelen_seek;
	size_t filelen_read;
	uint8_t *tmpbuf;

	ret = fopen_s(&f, filename, "rb");
	assert(0 == ret);

	ret = fseek(f, 0, SEEK_END);
	assert(0 == ret);

	// TODO
	filelen_seek = ftell(f);
	assert(filelen_seek >= 0);

	tmpbuf = (uint8_t *)malloc(filelen_seek);
	assert(NULL != tmpbuf);

	ret = fseek(f, 0, SEEK_SET);
	assert(0 == ret);

	filelen_read = fread(tmpbuf, 1, filelen_seek, f);
	assert(filelen_read == filelen_seek);

	ret = fclose(f);
	assert(0 == ret);

	*length = filelen_read;
	*buffer = tmpbuf;

	printf("[Loader] file loaded\n");

	return 0;
}