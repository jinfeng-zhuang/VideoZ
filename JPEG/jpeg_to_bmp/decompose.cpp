#include "jpeg_section.h"

struct jpeg_marker {
	const uint8_t update_section_length;
	const char *name;
};

struct jpeg_marker jpeg_marker_array[] = {
	{0xC0, "SOF"},
	{0xC4, "DHT"},
	{0xD8, "Start of image"},
	{0xD9, "End of image"},
	{0xDA, "Start of scan"},
	{0xDB, "DQT"},
	{0xE0, "APP"},
	{0xE1, "APP"},
};

struct jpeg_marker jpeg_marker_array_important[] = {
	{ 0xC0, "SOF" },
	{ 0xC4, "DHT" },
	{ 0xD9, "EOI" },
	{ 0xDA, "SOS" },
	{ 0xDB, "DQT" },
};

const char *jpeg_marker_search(uint8_t update_section_length)
{
	for (int i = 0; i < sizeof(jpeg_marker_array) / sizeof(jpeg_marker_array[0]); i++) {
		if (jpeg_marker_array[i].update_section_length == update_section_length) {
			return jpeg_marker_array[i].name;
		}
	}

	return "Unknown";
}

const char *jpeg_marker_search_important(uint8_t update_section_length)
{
	for (int i = 0; i < sizeof(jpeg_marker_array_important) / sizeof(jpeg_marker_array_important[0]); i++) {
		if (jpeg_marker_array_important[i].update_section_length == update_section_length) {
			return jpeg_marker_array_important[i].name;
		}
	}

	return "Unknown";
}

static uint32_t section_offset_array[CONFIG_JPEG_SECTION_NUMBER];
static uint32_t section_size_array[CONFIG_JPEG_SECTION_NUMBER];

int decompress_jpeg(uint8_t *buffer, uint32_t length, struct jpeg_section *info)
{
	assert(NULL != buffer);
	assert(NULL != info);

	uint32_t i, j;
	int update_section_length = 0;
	uint32_t first_eoi = 0;
	int eoi_number = 0;

	// display all the sections
	for (i = 0; i<length; i++) {
		if ((0xFF == buffer[i]) && (0 != buffer[i + 1])) {
			printf("[DECOMPOSE] Section: 0xFF %02X: %s\n", buffer[i + 1], jpeg_marker_search(buffer[i + 1]));

			if (0xD9 == buffer[i + 1]) {
				eoi_number++;
			}

			if ((1 == eoi_number) && (0 == first_eoi)) {
				first_eoi = i + 2;
			}
		}
	}

	if (1 == eoi_number) {
		first_eoi = 0;
	}

	printf("\n\nIMPORTANT SECTIONS\n\n");

	// section number
	for (i= first_eoi, j=0; i<length; i++) {
		if ((0xFF == buffer[i]) && (0 != buffer[i + 1])) {

			if (update_section_length) {
				update_section_length = 0;
				info[j].length = &buffer[i] - info[j].buffer;
				j++;
			}

			if (0 != strcmp("Unknown", jpeg_marker_search_important(buffer[i + 1]))) {
				if ("EOI" == jpeg_marker_search_important(buffer[i + 1])) {
					break;
				}

				info[j].type = buffer[i+1];
				info[j].buffer = &buffer[i];

				update_section_length = 1;

				assert(j <= CONFIG_JPEG_SECTION_NUMBER);

				printf("[DECOMPOSE] Section: 0xFF %02X: %s\n", buffer[i + 1], jpeg_marker_search(buffer[i + 1]));
			}
		}
	}

	return 0;
}