/*
 * The jpeg_decoder.h includes the exported symbols and required types in decoder.
 */

#ifndef JPEG_DECODER_H
#define JPEG_DECODER_H

#include "config.h"

enum {
	COLOR_COMPONENT_Y = 0,
	COLOR_COMPONENT_CB,
	COLOR_COMPONENT_CR,
};

enum {
	QUANTIZATION_TABLE_ID_Y = 0,
	QUANTIZATION_TABLE_ID_CB = 1,
	QUANTIZATION_TABLE_ID_CR = 1,
};

struct huffman_tree {
	int	count;
	uint8_t bitwidth[256]; // bitwidth[16][N]
	uint8_t symbol[256];
	uint16_t code[256];
	char bitstream[256][17];
};

struct jpeg_decoder {
	int width;
	int height;
	int Y_number;
	int mcu_width;
	int mcu_height;
	uint8_t color_to_quant[3];
	uint8_t color_to_huffman[3];
	struct huffman_tree huffman_tree[2][2]; // id : dc_ac
	int quantization_table[4][64];
};

/*
 * see jpeg_section_array
 */
struct jpeg_section {
	uint8_t type;
	uint8_t *buffer;
	uint32_t length;
};

// TODO remove it
struct huffman_symbol {
	uint8_t symbol;
	uint8_t bit_width;
	uint16_t huffman_code;
	uint8_t huffman_bitstream[17];
};

// seems huffman_tree[dc/ac][id] is more suitable
struct huffman_db {
	int number;
	uint8_t index[4];
	int symbol_number[4];
	struct huffman_symbol *symbol[4];
};

/*
 * for each MCU block's decode
 */
enum {
	HUFFMAN_TREE_ID_0 = 0,
	HUFFMAN_TREE_ID_1 = 1
};

extern int decompress_jpeg(uint8_t *buffer, uint32_t length, struct jpeg_section *info);

extern int jpeg_sof_decode(uint8_t *buffer, uint32_t length, struct jpeg_decoder *info);
extern int jpeg_dht_decode(uint8_t *buffer, uint32_t length, struct jpeg_decoder *info);
extern int jpeg_dqt_decode(uint8_t *buffer, uint32_t length, struct jpeg_decoder *info);
extern int jpeg_sos_decode(uint8_t *buffer, uint32_t length, struct jpeg_decoder *info);

extern int jpeg_huffman_tree_search(
	struct huffman_tree *tree,
	int huffman_code_width,
	uint16_t huffman_code,
	uint8_t *symbol
	);

extern int jpeg_canonical_huffman_decode(uint8_t bit_width, uint16_t huffman_code);

extern int jpeg_inverse_quantization(int *src, int *dst, int *table);

extern void jpeg_block_idct(int* src, unsigned char* dst);

#endif