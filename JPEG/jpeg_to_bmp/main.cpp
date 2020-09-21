// MY.cpp : Defines the entry point for the console application.
//

#include "jpeg_section.h"
#include "bmp_codec.h"

uint8_t *frame;
int frame_width;
int frame_height;

static struct jpeg_section jpeg_section_array[CONFIG_JPEG_SECTION_NUMBER];
static struct jpeg_decoder jpeg_decoder_info;

// assume
// put 16x16 block to a container
// unit = 3
// fw is 16xN
// fh is 16xN
int output(uint8_t *matrix)
{
	assert(NULL != matrix);
	assert(NULL != frame);
	
	assert(frame_width % jpeg_decoder_info.mcu_width == 0);
	assert(frame_height % jpeg_decoder_info.mcu_height == 0);

	int width = jpeg_decoder_info.mcu_width;
	int height = jpeg_decoder_info.mcu_height;

	static int x=0, y=0;

	int base = y * frame_width + x;

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			frame[(base + row * frame_width + col)*3 + 2] = matrix[(row * width + col)*3 + 0]; // RGB -> BGR
			frame[(base + row * frame_width + col)*3 + 1] = matrix[(row * width + col)*3 + 1];
			frame[(base + row * frame_width + col)*3 + 0] = matrix[(row * width + col)*3 + 2];
		}
	}

	if ((x + width) / frame_width) {
		y += height;
		x = 0;
	}
	else {
		x += width;
	}

	return 0;
}

int tranverse(uint8_t *frame)
{
	assert(NULL != frame);

	uint8_t tmp;

	for (int s_row = 0, d_row = frame_height-1; s_row <= d_row; s_row++, d_row--) {
		for (int col = 0; col < frame_width * 3; col++) {
			tmp = frame[s_row * frame_width * 3 + col]; // TODO, buffer++, optimize time
			frame[s_row * frame_width * 3 + col] = frame[d_row * frame_width * 3 + col];
			frame[d_row * frame_width * 3 + col] = tmp;
		}
	}

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int ret;
	FILE *fp;

	ret = fopen_s(&fp, "output.rgb24.bmp", "wb");
	assert(0 == ret);
	assert(NULL != fp);

	const char *filename = "test.jpg";
	uint8_t *buffer = NULL;
	uint32_t length = 0;

	ret = load_file("test.jpg", &buffer, &length);
	assert(0 == ret);

	ret = decompress_jpeg(buffer, length, jpeg_section_array);
	assert(0 == ret);

	for (uint32_t i = 0; i < CONFIG_JPEG_SECTION_NUMBER; i++) {
		if (JPEG_SECTION_SOF == jpeg_section_array[i].type) {
			ret = jpeg_sof_decode(jpeg_section_array[i].buffer, jpeg_section_array[i].length, &jpeg_decoder_info);
			assert(0 == ret);
		}
	}

	int mcu_width = jpeg_decoder_info.mcu_width;
	int mcu_height = jpeg_decoder_info.mcu_height;
	frame_width = ((jpeg_decoder_info.width / mcu_width) + (jpeg_decoder_info.width % mcu_width ? 1 : 0)) * mcu_width;
	frame_height = ((jpeg_decoder_info.height / mcu_height) + (jpeg_decoder_info.height % mcu_height ? 1 : 0)) * mcu_height;
	frame = (uint8_t *)malloc(frame_width * frame_height * 3);
	assert(NULL != frame);

	for (uint32_t i = 0; i < CONFIG_JPEG_SECTION_NUMBER; i++) {
		if (JPEG_SECTION_DQT == jpeg_section_array[i].type) {
			ret = jpeg_dqt_decode(jpeg_section_array[i].buffer, jpeg_section_array[i].length, &jpeg_decoder_info);
			assert(0 == ret);
		}
	}

	for (uint32_t i = 0; i < CONFIG_JPEG_SECTION_NUMBER; i++) {
		if (JPEG_SECTION_DHT == jpeg_section_array[i].type) {
			ret = jpeg_dht_decode(jpeg_section_array[i].buffer, jpeg_section_array[i].length, &jpeg_decoder_info);
			assert(0 == ret);
		}
	}

	for (uint32_t i = 0; i < CONFIG_JPEG_SECTION_NUMBER; i++) {
		if (JPEG_SECTION_SOS == jpeg_section_array[i].type) {
			ret = jpeg_sos_decode(jpeg_section_array[i].buffer, jpeg_section_array[i].length, &jpeg_decoder_info);
			assert(0 == ret);
		}
	}

	ret = bmp_prepare_header(fp, frame_width, frame_height);
	assert(0 == ret);

	ret = tranverse(frame);
	assert(0 == ret);

	ret = fwrite(frame, 1, frame_width * frame_height * 3, fp);
	assert(ret == frame_width * frame_height * 3);
	
	ret = fclose(fp);
	assert(0 == ret);

	return 0;
}