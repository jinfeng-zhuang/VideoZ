#include "jpeg_section.h"

int jpeg_sof_decode(uint8_t *buffer, uint32_t length, struct jpeg_decoder *info)
{
	assert(NULL != buffer);

	struct jpeg_marker_sof *sof = (struct jpeg_marker_sof *)buffer;

	//TODO
	//assert(length == BIGENDIAN_16(sof->length) + 2);
	
	assert(3 == sof->color_component_number);

	assert((sof->color_component_array[0].id == 0) || (sof->color_component_array[0].id == 1));
	assert(sof->color_component_array[0].quanid == 0);

	info->Y_number = sof->color_component_array[0].h_sample * sof->color_component_array[0].v_sample;

	if (1 == sof->color_component_array[0].h_sample) {
		assert(1 == sof->color_component_array[0].v_sample);
	}

	if (sof->color_component_array[0].h_sample == 2) {
		info->mcu_width = 16;
	}
	else {
		info->mcu_width = 8;
	}

	if (sof->color_component_array[0].v_sample == 2) {
		info->mcu_height = 16;
	}
	else {
		info->mcu_height = 8;
	}

	assert((sof->color_component_array[1].id == 1) || (sof->color_component_array[1].id == 2));
	assert(sof->color_component_array[1].quanid == 1);
	assert(sof->color_component_array[1].h_sample == 1);
	assert(sof->color_component_array[1].v_sample == 1);

	assert((sof->color_component_array[2].id == 2) || (sof->color_component_array[2].id == 3));
	assert(sof->color_component_array[2].quanid == 1);
	assert(sof->color_component_array[2].h_sample == 1);
	assert(sof->color_component_array[2].v_sample == 1);

	info->color_to_quant[0] = sof->color_component_array[0].quanid;
	info->color_to_quant[1] = sof->color_component_array[1].quanid;
	info->color_to_quant[2] = sof->color_component_array[2].quanid;

	printf("[SOF] Precision: %d, Width : %d, Height : %d\n", sof->precision, BIGENDIAN_16(sof->width), BIGENDIAN_16(sof->height));
	
	info->width = BIGENDIAN_16(sof->width);
	info->height = BIGENDIAN_16(sof->height);

	return 0;
}