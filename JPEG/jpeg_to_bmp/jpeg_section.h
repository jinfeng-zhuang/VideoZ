#ifndef JPEG_SECTION_H
#define JPEG_SECTION_h

#include <stdint.h>

enum {
	HUFFMAN_TREE_ID_0 = 0,
	HUFFMAN_TREE_ID_1 = 1
};

enum {
	COLOR_COMPONENT_Y,
	COLOR_COMPONENT_C
};

enum {
	HUFFMAN_Y_DC = 0x00,
	HUFFMAN_Y_AC = 0x10,
	HUFFMAN_C_DC = 0x01,
	HUFFMAN_C_AC = 0x11
};

enum {
	JPEG_SECTION_DQT = 0xDB,
	JPEG_SECTION_SOF = 0xC0,
	JPEG_SECTION_DHT = 0xC4,
	JPEG_SECTION_SOS = 0xDA,
};

struct jpeg_section {
	uint8_t type;
	uint32_t length;
	uint8_t *buffer;
};

struct huffman_symbol {
	uint8_t symbol;
	uint8_t bit_width;
	uint16_t huffman_code;
	uint8_t huffman_bitstream[17];
};

struct huffman_db {
	int number;
	uint8_t index[4];
	int symbol_number[4];
	struct huffman_symbol *symbol[4];
};

#pragma pack(push)
#pragma pack(1)

// FIXME: id low 4 bit
struct jpeg_marker_dqt_data {
	uint8_t id : 4;
	uint8_t precision : 4;
	unsigned char data[64];
};

struct jpeg_marker_dqt {
	uint16_t flag;
	uint16_t length;
	struct jpeg_marker_dqt_data table[1]; // extend
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
	uint8_t huffman_table_index;
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

struct bmp_file_header {
	uint16_t type;
	uint32_t size;
	uint16_t rsvd1;
	uint16_t rsvd2;
	uint32_t rgb_offset;
};

struct bmp_image_header {
	uint32_t header_size;
	uint32_t width;
	uint32_t height;
	uint16_t planes;
	uint16_t bits_per_pixel;
	uint32_t compress;
	uint32_t size;
	uint32_t xpixel_per_meter;
	uint32_t ypixel_per_meter;
	uint32_t color_used;
	uint32_t important_color;
};

#pragma pack(pop)

#define BIGENDIAN_16(x) (((x>>8)&0xFF) | ((x&0xFF)<<8))

/******************************************************************************************************************/

extern int load_file(const char *filename, uint8_t **buffer, uint32_t *length);
extern int decompress_jpeg(uint8_t *buffer, uint32_t length, struct jpeg_section **sections, uint32_t *number);

extern int quantization_resume(uint8_t *buffer, uint32_t length);
extern int quantization_apply(int *src, int *dst, int quan_id);

extern int huffman_decoder(uint8_t *buffer, uint32_t length, struct huffman_db *huffman_table);
extern int huffman_table_dump(struct huffman_db *db, uint8_t index);
extern int huffman_table_search(struct huffman_db *db, uint8_t index, int huffman_code_width, uint16_t huffman_code, uint8_t *symbol);

extern int sos_analyze(uint8_t *buffer, uint32_t length);

extern int SOF_Analyze(uint8_t *buffer, uint32_t length);

extern int canonical_huffman_decode(uint8_t bit_width, uint16_t huffman_code);

extern void block_idct(int* src, unsigned char* dst);

extern int output(uint8_t *matrix);

/******************************************************************************************************************/

extern struct huffman_db huffman_table;
extern uint8_t zigzag_array_reverse[];

#endif