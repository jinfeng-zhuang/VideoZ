#ifndef BMP_H
#define BMP_H

#include <stdint.h>

#pragma pack(push)
#pragma pack(1)

struct bmp_file_header {
	uint16_t type;
	uint32_t size;
	uint16_t rsvd1;
	uint16_t rsvd2;
	uint32_t rgb_offset;
};

struct bmp_image_header {
	uint32_t header_size;
	uint32_t width;
	uint32_t height;
	uint16_t planes;
	uint16_t bits_per_pixel;
	uint32_t compress;
	uint32_t size;
	uint32_t xpixel_per_meter;
	uint32_t ypixel_per_meter;
	uint32_t color_used;
	uint32_t important_color;
};

#pragma pack(pop)

#endif