#include "stdafx.h"

#include "jpeg_section.h"

const uint16_t IPSF[64] = {
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

struct jpeg_marker_dqt *dqt = NULL;

int dqt_dump(const char *title, int *buffer)
{
	printf("\n%s\n", title);

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			printf("%d ", buffer[i * 8 + j]);
		}
		printf("\n");
	}

	printf("\n");

	return 0;
}

int quantization_table[4][64];

int quantization_resume(uint8_t *buffer, uint32_t length)
{
	assert(NULL != buffer);

	int id;
	dqt = (struct jpeg_marker_dqt *)buffer;

	assert(0 == dqt->table[0].precision);

	id = dqt->table[0].id;
	assert(id < 4);

	//dqt_dump("DQT", dqt->table[0].data);

	int index;

	for (int i = 0; i < 64; i++) {
		index = zigzag_array_reverse[i];
		quantization_table[id][index] = (int32_t)((uint32_t)dqt->table[0].data[i] * IPSF[index]); // TODO
		//quantization_table[id][i] = (int32_t)((uint32_t)dqt->table[0].data[i] * IPSF[i]); // TODO
	}

	//dqt_dump("DQT After Scale", (uint8_t *)quantization_table);

	return 0;
}

int quantization_apply(int *src, int *dst, int quan_id)
{
	assert(NULL != src);
	assert(quan_id < 4);
	assert(NULL != dqt);

	dqt_dump("DQT", quantization_table[quan_id]);

	for (int i = 0; i < 64; i++) {
		int index = zigzag_array_reverse[i];
		dst[index] = src[i] * quantization_table[quan_id][index] >> 8;
	}

	return 0;
}