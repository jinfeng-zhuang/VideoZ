#include "stdafx.h"
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#include "jpeg_section.h"

struct jpeg_flag {
	uint8_t flag;
	enum jpeg_section_type type;
	const char *name;
};

struct jpeg_section *jpeg_section_array;
uint32_t *section_offset_array;
uint32_t *section_size_array;

int decompress_jpeg(uint8_t *buffer, uint32_t length, struct jpeg_section **sections, uint32_t *number)
{
	assert(NULL != buffer);
	assert(NULL == *sections);
	assert(0 == *number);

	uint32_t i;
	int j;
	uint32_t section_number = 0;
	uint32_t section_size_max = 0; /* MUST init as 0 */
	uint32_t index_flag;
	uint32_t index_data;

	// section number
	for (i=0; i<length; i++) {
		if ((0xFF == buffer[i]) && (0x00 != buffer[i+1])) {
			section_number++;
			printf("SECTION: 0xFF %02X\n", buffer[i + 1]);
		}
	}

	// section start address
	section_offset_array = (uint32_t *)malloc(sizeof(uint32_t) * section_number);
	assert(NULL != section_offset_array);

	for (i=0, j=0; i<length; i++) {
		if ((0xFF == buffer[i]) && (0x00 != buffer[i + 1])) {
			section_offset_array[j] = i;
			j++;
		}
	}

	// section size
	section_size_array = (uint32_t *)malloc(sizeof(uint32_t) * section_number);
	assert(NULL != section_size_array);

	for (i=0; i<section_number; i++) {
		if (0 == i) {
			continue;
		}
		section_size_array[i-1] = section_offset_array[i] - section_offset_array[i-1];
	}
	section_size_array[i-1] = length - section_offset_array[i-1]; /* the last one */

	// section size max
	for (i=0; i<section_number; i++) {
		if (section_size_array[i] > section_size_max) {
			section_size_max = section_size_array[i];
		}
	}

	// section array allocate
	jpeg_section_array = (struct jpeg_section *)malloc(sizeof(struct jpeg_section) * section_number);
	assert(NULL != jpeg_section_array);
	
	for (i=0; i<section_number; i++) {
		jpeg_section_array[i].buffer = (uint8_t *)malloc(section_size_max);
		assert(NULL != jpeg_section_array[i].buffer);
	}

	// section fill content
	for (i=0; i<section_number; i++) {
		index_flag = section_offset_array[i] + 1;
		index_data = section_offset_array[i];

		jpeg_section_array[i].length = section_size_array[i];
		jpeg_section_array[i].type = buffer[index_flag];
		memcpy(jpeg_section_array[i].buffer, &buffer[index_data], jpeg_section_array[i].length);
	}

	// OUTPUT
	*number = section_number;
	*sections = jpeg_section_array;

	return 0;
}