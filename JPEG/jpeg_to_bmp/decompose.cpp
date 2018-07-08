#include "jpeg_section.h"

static uint32_t section_offset_array[CONFIG_JPEG_SECTION_NUMBER];
static uint32_t section_size_array[CONFIG_JPEG_SECTION_NUMBER];

int decompress_jpeg(uint8_t *buffer, uint32_t length, struct jpeg_section *info)
{
	assert(NULL != buffer);
	assert(NULL != info);

	uint32_t i, j;
	uint32_t section_number = 0;
	uint32_t offset_flag;
	uint32_t offset_data;

	// section number
	for (i=0; i<length; i++) {
		if ((0xFF == buffer[i]) && (0x00 != buffer[i+1])) {
			section_number++;
			printf("[DECOMPOSE] Section: 0xFF %02X\n", buffer[i + 1]);
		}
	}

	assert(section_number <= CONFIG_JPEG_SECTION_NUMBER);

	// section start address
	for (i=0, j=0; i<length; i++) {
		if ((0xFF == buffer[i]) && (0x00 != buffer[i + 1])) {
			section_offset_array[j] = i;
			j++;
		}
	}

	// section size
	for (i=0; i<section_number; i++) {
		if (0 == i) {
			continue;
		}
		section_size_array[i-1] = section_offset_array[i] - section_offset_array[i-1];
	}
	section_size_array[i-1] = length - section_offset_array[i-1]; /* the last one */

	// section fill content
	for (i=0; i<section_number; i++) {
		offset_flag = section_offset_array[i] + 1;
		offset_data = section_offset_array[i];

		info[i].length = section_size_array[i];
		info[i].type = buffer[offset_flag];
		info[i].buffer = &buffer[offset_data];
	}

	return 0;
}