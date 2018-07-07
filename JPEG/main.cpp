// MY.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "jpeg_section.h"

FILE *fp;

// 16 x 16 x 3 => RGB 16 x 16 pixels
int output(uint8_t *matrix)
{
	assert(NULL != matrix);
	assert(NULL != fp);

	int ret;

	ret = fwrite(matrix, 1, 16 * 16 * 3, fp);
	assert(ret == 16 * 16 * 3);

	ret = fflush(fp);
	assert(0 == ret);

	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int ret;

	ret = fopen_s(&fp, "output.rgb24", "wb");
	assert(0 == ret);

	const char *filename = "test.jpg";
	uint8_t *buffer = NULL;
	uint32_t length = 0;

	struct jpeg_section *sections = NULL;
	uint32_t section_number = 0;

	ret = load_file("test.jpg", &buffer, &length);
	assert(0 == ret);

	ret = decompress_jpeg(buffer, length, &sections, &section_number);
	assert(0 == ret);

	for (uint32_t i = 0; i < section_number; i++) {
		if (JPEG_SECTION_SOF == sections[i].type) {
			ret = SOF_Analyze(sections[i].buffer, sections[i].length);
			assert(0 == ret);
		}
	}

	for (uint32_t i = 0; i < section_number; i++) {
		if (JPEG_SECTION_DQT == sections[i].type) {
			ret = quantization_resume(sections[i].buffer, sections[i].length);
			assert(0 == ret);
		}
	}

	for (uint32_t i = 0; i < section_number; i++) {
		if (JPEG_SECTION_DHT == sections[i].type) {
			ret = huffman_decoder(sections[i].buffer, sections[i].length, &huffman_table);
			assert(0 == ret);
		}
	}

	for (uint32_t i = 0; i < section_number; i++) {
		if (JPEG_SECTION_SOS == sections[i].type) {
			ret = sos_analyze(sections[i].buffer, sections[i].length);
			assert(0 == ret);
		}
	}

	assert(NULL != fp);
	ret = fclose(fp);
	assert(0 == ret);

	return 0;
}