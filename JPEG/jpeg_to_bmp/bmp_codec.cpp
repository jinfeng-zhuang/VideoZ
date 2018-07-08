#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "bmp.h"
#include "bmp_codec.h"

int bmp_prepare_header(FILE *fp, int width, int height)
{
	assert(NULL != fp);

	int ret;

	struct bmp_file_header file;
	struct bmp_image_header image;

	memset(&file, 0, sizeof(struct bmp_file_header));
	memset(&image, 0, sizeof(struct bmp_image_header));

	file.type = 'MB';
	file.size = sizeof(file) + sizeof(image) + width * height * 3;
	file.rgb_offset = sizeof(file) + sizeof(image);

	image.header_size = sizeof(image);
	image.width = width;
	image.height = height;
	image.planes = 1;
	image.bits_per_pixel = 24;
	image.compress = 0;

	ret = fwrite(&file, 1, sizeof(file), fp);
	assert(ret == sizeof(file));

	ret = fwrite(&image, 1, sizeof(image), fp);
	assert(ret == sizeof(image));

	return 0;
}