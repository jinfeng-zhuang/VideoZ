/*
 * The jpeg_decoder.h includes the exported symbols and required types in decoder.
 */

#ifndef JPEG_DECODER_H
#define JPEG_DECODER_H

/*
 * see jpeg_section_array
 */
struct jpeg_section {
	uint8_t type;
	uint32_t length;
	uint8_t *buffer;
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

extern int jpeg_sof_decode(uint8_t *buffer, uint32_t length);
extern int jpeg_dht_decode(uint8_t *buffer, uint32_t length, struct huffman_db *huffman_table);
extern int jpeg_dqt_decode(uint8_t *buffer, uint32_t length);
extern int jpeg_sos_decode(uint8_t *buffer, uint32_t length);

extern int jpeg_huffman_tree_search(struct huffman_db *db, uint8_t index, int huffman_code_width, uint16_t huffman_code, uint8_t *symbol);

extern int jpeg_canonical_huffman_decode(uint8_t bit_width, uint16_t huffman_code);

extern int jpeg_inverse_quantization(int *src, int *dst, int quan_id);

extern void jpeg_block_idct(int* src, unsigned char* dst);

#endif