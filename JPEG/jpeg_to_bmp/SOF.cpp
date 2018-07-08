#include "jpeg_section.h"

int jpeg_sof_decode(uint8_t *buffer, uint32_t length)
{
	assert(NULL != buffer);

	struct jpeg_marker_sof *sof = (struct jpeg_marker_sof *)buffer;

	assert(length == BIGENDIAN_16(sof->length) + 2);
	
	assert(3 == sof->color_component_number);
	assert(sof->color_component_array[0].h_sample == 2);
	assert(sof->color_component_array[1].h_sample == 1);
	assert(sof->color_component_array[2].h_sample == 1);

	printf("[SOF] precision: %d width = %d height = %d\n", sof->precision, BIGENDIAN_16(sof->width), BIGENDIAN_16(sof->height));
	

	return 0;
}