// MY.cpp : Defines the entry point for the console application.
//

#include "jpeg_section.h"
#include "bmp_codec.h"

#define FRAME_WIDTH		(128)
#define FRAME_HEIGHT	(128)
#define FRAME_SIZE		(FRAME_WIDTH * FRAME_HEIGHT * 3)
uint8_t frame[FRAME_SIZE] = { 0 };

static struct jpeg_section jpeg_section_array[CONFIG_JPEG_SECTION_NUMBER];

// assume
// put 16x16 block to a container
// unit = 3
// fw is 16xN
// fh is 16xN
int output(uint8_t *matrix)
{
	assert(NULL != matrix);
	assert(NULL != frame);
	
	assert(FRAME_WIDTH % 16 == 0);
	assert(FRAME_HEIGHT % 16 == 0);

#define BLOCK_WIDTH		(16)
#define BLOCK_HEIGHT	(16)
#define UNIT_SIZE		(3)

	static int x=0, y=0;

	int base = y * FRAME_WIDTH + x;

	for (int row = 0; row < BLOCK_HEIGHT; row++) {
		for (int col = 0; col < BLOCK_WIDTH; col++) {
			frame[(base + row * FRAME_WIDTH + col)*UNIT_SIZE + 2] = matrix[(row * BLOCK_WIDTH + col)*UNIT_SIZE + 0]; // RGB -> BGR
			frame[(base + row * FRAME_WIDTH + col)*UNIT_SIZE + 1] = matrix[(row * BLOCK_WIDTH + col)*UNIT_SIZE + 1];
			frame[(base + row * FRAME_WIDTH + col)*UNIT_SIZE + 0] = matrix[(row * BLOCK_WIDTH + col)*UNIT_SIZE + 2];
		}
	}

	if ((x + BLOCK_WIDTH) / FRAME_WIDTH) {
		y += BLOCK_HEIGHT;
		x = 0;
	}
	else {
		x += BLOCK_WIDTH;
	}

	return 0;
}

int tranverse(uint8_t *frame)
{
	assert(NULL != frame);

	uint8_t tmp;

	for (int s_row = 0, d_row = FRAME_HEIGHT-1; s_row <= d_row; s_row++, d_row--) {
		for (int col = 0; col < FRAME_WIDTH * 3; col++) {
			tmp = frame[s_row * FRAME_WIDTH * 3 + col]; // TODO, buffer++, optimize time
			frame[s_row * FRAME_WIDTH * 3 + col] = frame[d_row * FRAME_WIDTH * 3 + col];
			frame[d_row * FRAME_WIDTH * 3 + col] = tmp;
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
			ret = jpeg_sof_decode(jpeg_section_array[i].buffer, jpeg_section_array[i].length);
			assert(0 == ret);
		}
	}

	for (uint32_t i = 0; i < CONFIG_JPEG_SECTION_NUMBER; i++) {
		if (JPEG_SECTION_DQT == jpeg_section_array[i].type) {
			ret = jpeg_dqt_decode(jpeg_section_array[i].buffer, jpeg_section_array[i].length);
			assert(0 == ret);
		}
	}

	for (uint32_t i = 0; i < CONFIG_JPEG_SECTION_NUMBER; i++) {
		if (JPEG_SECTION_DHT == jpeg_section_array[i].type) {
			ret = jpeg_dht_decode(jpeg_section_array[i].buffer, jpeg_section_array[i].length, &huffman_table);
			assert(0 == ret);
		}
	}

	for (uint32_t i = 0; i < CONFIG_JPEG_SECTION_NUMBER; i++) {
		if (JPEG_SECTION_SOS == jpeg_section_array[i].type) {
			ret = jpeg_sos_decode(jpeg_section_array[i].buffer, jpeg_section_array[i].length);
			assert(0 == ret);
		}
	}

	ret = bmp_prepare_header(fp, FRAME_WIDTH, FRAME_HEIGHT);
	assert(0 == ret);

	ret = tranverse(frame);
	assert(0 == ret);

	ret = fwrite(frame, 1, FRAME_SIZE, fp);
	assert(ret == FRAME_SIZE);
	
	ret = fclose(fp);
	assert(0 == ret);

	return 0;
}