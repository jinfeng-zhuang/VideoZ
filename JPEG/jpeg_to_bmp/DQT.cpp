#include "jpeg_section.h"

static const uint16_t IPSF[64] = {
	(uint16_t)(1.00000 * 8192), (uint16_t)(1.38704 * 8192), (uint16_t)(1.30656 * 8192), (uint16_t)(1.17588 * 8192),
	(uint16_t)(1.00000 * 8192), (uint16_t)(0.78570 * 8192), (uint16_t)(0.54120 * 8192), (uint16_t)(0.27590 * 8192),
	(uint16_t)(1.38704 * 8192), (uint16_t)(1.92388 * 8192), (uint16_t)(1.81226 * 8192), (uint16_t)(1.63099 * 8192),
	(uint16_t)(1.38704 * 8192), (uint16_t)(1.08979 * 8192), (uint16_t)(0.75066 * 8192), (uint16_t)(0.38268 * 8192),
	(uint16_t)(1.30656 * 8192), (uint16_t)(1.81226 * 8192), (uint16_t)(1.70711 * 8192), (uint16_t)(1.53636 * 8192),
	(uint16_t)(1.30656 * 8192), (uint16_t)(1.02656 * 8192), (uint16_t)(0.70711 * 8192), (uint16_t)(0.36048 * 8192),
	(uint16_t)(1.17588 * 8192), (uint16_t)(1.63099 * 8192), (uint16_t)(1.53636 * 8192), (uint16_t)(1.38268 * 8192),
	(uint16_t)(1.17588 * 8192), (uint16_t)(0.92388 * 8192), (uint16_t)(0.63638 * 8192), (uint16_t)(0.32442 * 8192),
	(uint16_t)(1.00000 * 8192), (uint16_t)(1.38704 * 8192), (uint16_t)(1.30656 * 8192), (uint16_t)(1.17588 * 8192),
	(uint16_t)(1.00000 * 8192), (uint16_t)(0.78570 * 8192), (uint16_t)(0.54120 * 8192), (uint16_t)(0.27590 * 8192),
	(uint16_t)(0.78570 * 8192), (uint16_t)(1.08979 * 8192), (uint16_t)(1.02656 * 8192), (uint16_t)(0.92388 * 8192),
	(uint16_t)(0.78570 * 8192), (uint16_t)(0.61732 * 8192), (uint16_t)(0.42522 * 8192), (uint16_t)(0.21677 * 8192),
	(uint16_t)(0.54120 * 8192), (uint16_t)(0.75066 * 8192), (uint16_t)(0.70711 * 8192), (uint16_t)(0.63638 * 8192),
	(uint16_t)(0.54120 * 8192), (uint16_t)(0.42522 * 8192), (uint16_t)(0.29290 * 8192), (uint16_t)(0.14932 * 8192),
	(uint16_t)(0.27590 * 8192), (uint16_t)(0.38268 * 8192), (uint16_t)(0.36048 * 8192), (uint16_t)(0.32442 * 8192),
	(uint16_t)(0.27590 * 8192), (uint16_t)(0.21678 * 8192), (uint16_t)(0.14932 * 8192), (uint16_t)(0.07612 * 8192)
};

static struct jpeg_marker_dqt *dqt = NULL;
static int quantization_table[4][64];

static int matrix_dump(const char *title, int *buffer)
{
	printf("\n%s\n", title);

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			printf("%d\t", buffer[i * 8 + j]);
		}
		printf("\n");
	}

	printf("\n");

	return 0;
}

int jpeg_dqt_decode(uint8_t *buffer, uint32_t length, struct jpeg_decoder *info)
{
	assert(NULL != buffer);
	assert(NULL != info);

	int id;
	dqt = (struct jpeg_marker_dqt *)buffer;

	assert(BIGENDIAN_16(dqt->length) + 2 == length);

	assert(0 == dqt->table[0].precision);

	id = dqt->table[0].id;
	assert(id < 4);

	printf("[JPEG DQT] ID : %d\n", id);

	int index;

	for (int i = 0; i < 64; i++) {
		index = zigzag_array_reverse[i];
		info->quantization_table[id][index] = (int32_t)((uint32_t)dqt->table[0].data[i] * IPSF[index]); // TODO
		//quantization_table[id][i] = (int32_t)((uint32_t)dqt->table[0].data[i] * IPSF[i]); // TODO
	}

	return 0;
}

int jpeg_inverse_quantization(int *src, int *dst, int *table)
{
	assert(NULL != src);
	assert(NULL != dst);
	assert(NULL != table);

	matrix_dump("DQT", table);

	// TODO
	for (int i = 0; i < 64; i++) {
		int index = zigzag_array_reverse[i];
		dst[index] = src[i] * table[index] >> 8;
	}

	matrix_dump("de-quantization", dst);

	return 0;
}