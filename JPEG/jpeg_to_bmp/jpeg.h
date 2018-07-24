#ifndef JPEG_H
#define JPEG_H

#include <stdint.h>

enum {
	JPEG_SECTION_DQT = 0xDB,
	JPEG_SECTION_SOF = 0xC0,
	JPEG_SECTION_DHT = 0xC4,
	JPEG_SECTION_SOS = 0xDA,
};

#pragma pack(push)
#pragma pack(1)

struct jpeg_marker_dqt {
	uint16_t flag;
	uint16_t length;
	struct jpeg_marker_dqt_data {
        uint8_t id : 4; // FIXME: id low 4 bit
        uint8_t precision : 4;
        unsigned char data[64];
    } table[1]; // extend
};

struct marker_sos {
	uint16_t flag;
	uint16_t length;
	uint8_t color_component_number;
	struct {
		uint8_t id;
		uint8_t huffman_table_ac_id : 4;
		uint8_t huffman_table_dc_id : 4;
	} color_component[3]; // TODO
	uint8_t start;
	uint8_t stop;
	uint8_t select;
	uint8_t data[1]; /* huffman data */
};

struct jpeg_marker_dht {
	uint16_t flag;
	uint16_t length;
	uint8_t id : 4;
	uint8_t ac_dc : 4;
	uint8_t number_of_level[16];
	uint8_t data[1];
};

struct jpeg_marker_sof {
	uint16_t flag;
	uint16_t length;
	uint8_t precision;
	uint16_t height;
	uint16_t width;
	uint8_t color_component_number;
	struct {
		uint8_t id;
		uint8_t v_sample : 4;
		uint8_t h_sample : 4;
		uint8_t quanid;
	} color_component_array[3]; // DOC, to ARRAY[1]
};

#pragma pack(pop)

#endif