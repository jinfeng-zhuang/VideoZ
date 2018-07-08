#include "stdafx.h"

#include "jpeg_section.h"

#define BIT(n)	(1<<(n))

int bitshift(const uint8_t *src, uint32_t *position)
{
	uint32_t pos = *position;
	uint32_t offset_byte = pos >> 3;
	uint32_t offset_bit_in_byte = pos & 0x7;
	uint8_t value;

	value = src[offset_byte] & (1 << (7 - offset_bit_in_byte));

	*position = *position + 1;

	return value ? 1 : 0;
}

int huffman_encoded_data_scan(uint8_t *sos_data_area, uint32_t *offset, struct huffman_db *db, uint8_t index, uint8_t *value)
{
	int i;
	int bit;
	uint16_t code = 0; /* start from 0 */ 
	uint8_t code_width;
	int ret;
	uint8_t symbol;

	for (i = 0; i < 16; i++) {
		bit = bitshift(sos_data_area, offset);

		code = (code << 1) + bit;
		code_width = i + 1;

		// TODO reconstruct the huffman_table for common usage
		ret = huffman_table_search(db, index, code_width, code, &symbol);
		if (0 == ret) {
			break;
		}
	}

	if (i >= 16) {
		return -1;
	}

	*value = symbol;

	return 0;
}

// TODO correct the function name
int rof_decode(uint8_t *sos_data_area, uint32_t *offset, struct huffman_db *db, uint8_t index, int *zeros, int *value)
{
	assert(NULL != sos_data_area);
	assert(NULL != offset);
	assert(NULL != db);
	assert(NULL != value);

	int ret;
	uint8_t symbol;

	ret = huffman_encoded_data_scan(sos_data_area, offset, db, index, &symbol);
	assert(0 == ret);

	if (0x00 == symbol) {
		return 1; // COMPLETE
	}

	int zero_before_it = (symbol >> 4) & 0xF;
	int bits_to_be_read = symbol & 0xF;
	uint16_t huffman_code = 0;

	for (int j = 0; j < bits_to_be_read; j++) {
		int bit = bitshift(sos_data_area, offset);
		huffman_code = (huffman_code << 1) | bit;
	}

	int dc_ac_value = canonical_huffman_decode(bits_to_be_read, huffman_code);

	*zeros = zero_before_it;
	*value = dc_ac_value;

	return 0;
}

int matrix[64];
int matrix_reverse[64]; // TODO
int matrix_quan[64];

uint8_t zigzag_array[64] = {
	1,  2,  6,  7, 15, 16, 28, 29,
	3,  5,  8, 14, 17, 27, 30, 43,
	4,  9, 13, 18, 26, 31, 42, 44,
	10, 12, 19, 25, 32, 41, 45, 54,
	11, 20, 24, 33, 40, 46, 53, 55,
	21, 23, 34, 39, 47, 52, 56, 61,
	22, 35, 38, 48, 51, 57, 60, 62,
	36, 37, 49, 50, 58, 59, 63, 64,
};

// HOWTO
uint8_t zigzag_array_reverse[64] = {
	0,  1,  8,  16, 9,  2,  3,  10,
	17, 24, 32, 25, 18, 11, 4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13, 6,  7,  14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63
};

int matrix_dump(const char *title, int *buffer)
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

int matrix_dump_8(const char *title, unsigned char *buffer)
{
	printf("\n%s\n", title);

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			printf("%02x ", buffer[i * 8 + j]);
		}
		printf("\n");
	}

	printf("\n");

	return 0;
}

int sos_mcu_block_dehuffman(uint8_t *sos_data_area, uint32_t *offset, struct huffman_db *db, uint8_t huffman_tree_id, int color)
{
	assert(color < 4);

	int ret;
	int zeros;
	int value;
	static int previous_dc_value_array[3] = { 0,0,0 };

	// TAKE ATTENTION,  sizeof(matrix)
	memset(matrix, 0, sizeof(matrix));

	// 1 DC coefficient
	ret = rof_decode(sos_data_area, offset, db, huffman_tree_id | 0x00, &zeros, &value);
	if (-1 == ret) {
		assert(0); // TODO
	}

	// TODO
	if (1 == ret) {
		value = 0;
	}

	value = value + previous_dc_value_array[color];
	previous_dc_value_array[color] = value;
	matrix[0] = value;

	// 63 AC coefficient
	for (int i = 1; i < 64; i++) {
		ret = rof_decode(sos_data_area, offset, db, huffman_tree_id | 0x10, &zeros, &value);
		if (-1 == ret) {
			assert(0); // TODO
		}
		else if (1 == ret) {
			goto complete;
		}

		i = i + zeros;

		// note: matrix init as 0
		matrix[i] = value;
	}

complete:

	matrix_dump("Original Matrix", matrix);

	return 0;
}

unsigned char matrix_idct[8*8];
uint8_t matrix_ycbcr[64 * 6];
unsigned char matrix_rgba[16*16 * 3];

int matrix_rgba_dump(uint8_t *matrix)
{
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			printf("%02x ", matrix[i * 16 * 3 + j * 3 + 0]);
			printf("%02x ", matrix[i * 16 * 3 + j * 3 + 1]);
			printf("%02x ", matrix[i * 16 * 3 + j * 3 + 2]);
		}
		printf("\n");
	}

	printf("\n");

	return 0;
}

// TODO optimize
uint8_t YC2RGB_Value_Convert(int tmp)
{
	uint32_t value;

	value = ((uint32_t)tmp) & 0x3FF;

	if (value & 0x200) {
		return 0x0;
	}
	else if (value & 0x100) {
		return 0xFF;
	}
	else {
		return value & 0xFF;
	}
}

// TODO Refer to figure
int YCbCr_to_RGB_411(uint8_t *YCbCr, uint8_t *RGB)
{
	assert(NULL != YCbCr);
	assert(NULL != RGB);

	uint8_t *Y_buf_array[4] = { YCbCr, YCbCr + 64, YCbCr + 64 * 2, YCbCr + 64 * 3 };
	uint8_t *Y_buf;
	uint8_t *Cb_buf = YCbCr + 64 * 4;
	uint8_t *Cr_buf = YCbCr + 64 * 5;

	int C_index;
	int Y_index;

	int Y, Cb, Cr;

	for (int y = 0; y < 16; y++) {
		for (int x = 0; x < 16; x++) {
			Y_buf = Y_buf_array[(y / 8) * 2 + (x / 8)];

			Y_index = (y % 8) * 8 + (x % 8);
			C_index = (y >> 1) * 8 + (x >> 1);

			Y = Y_buf[Y_index];
			Cb = Cb_buf[C_index] - 128; // TODO -128
			Cr = Cr_buf[C_index] - 128;

			// SEQ: RGB, BMP SEQ: BGR
			RGB[y * 16 * 3 + x * 3 + 0] = YC2RGB_Value_Convert(Y + ((int)(1.402 * 1024) * Cr) / 1024);
			RGB[y * 16 * 3 + x * 3 + 1] = YC2RGB_Value_Convert(Y - ((int)(0.344 * 1024) * Cb + (int)(0.714 * 1024) * Cr) / 1024);
			RGB[y * 16 * 3 + x * 3 + 2] = YC2RGB_Value_Convert(Y + ((int)(1.772 * 1024) * Cb) / 1024);
		}
	}

	matrix_rgba_dump(RGB);

	output(RGB);

	return 0;
}

int sos_mcu_block_dequan(int *matrix, int quan_id, uint8_t *matrix_dst)
{
	int ret;

	ret = quantization_apply(matrix, matrix_quan, quan_id);
	assert(0 == ret);

	matrix_dump("De-Quantization Matrix", matrix_quan);

	block_idct(matrix_quan, matrix_dst);

	matrix_dump_8("IDCT result", matrix_dst);

	return 0;
}

int sos_mcu_parse(uint8_t *sos_data_area, uint32_t *offset)
{
	sos_mcu_block_dehuffman(sos_data_area, offset, &huffman_table, HUFFMAN_TREE_ID_0, 0);
	sos_mcu_block_dequan(matrix, 0, &matrix_ycbcr[64 * 0]);

	sos_mcu_block_dehuffman(sos_data_area, offset, &huffman_table, HUFFMAN_TREE_ID_0, 0);
	sos_mcu_block_dequan(matrix, 0, &matrix_ycbcr[64 * 1]);

	sos_mcu_block_dehuffman(sos_data_area, offset, &huffman_table, HUFFMAN_TREE_ID_0, 0);
	sos_mcu_block_dequan(matrix, 0, &matrix_ycbcr[64 * 2]);

	sos_mcu_block_dehuffman(sos_data_area, offset, &huffman_table, HUFFMAN_TREE_ID_0, 0);
	sos_mcu_block_dequan(matrix, 0, &matrix_ycbcr[64 * 3]);

	sos_mcu_block_dehuffman(sos_data_area, offset, &huffman_table, HUFFMAN_TREE_ID_1, 1);
	sos_mcu_block_dequan(matrix, 1, &matrix_ycbcr[64 * 4]);

	sos_mcu_block_dehuffman(sos_data_area, offset, &huffman_table, HUFFMAN_TREE_ID_1, 2);
	sos_mcu_block_dequan(matrix, 1, &matrix_ycbcr[64 * 5]);

	YCbCr_to_RGB_411(matrix_ycbcr, matrix_rgba);

	return 0;
}

int sos_analyze(uint8_t *buffer, uint32_t length)
{
	assert(NULL != buffer);

	int ret;
	uint32_t bit_offset = 0;
	
	printf("[SOS] section length = %d\n", length);

	struct marker_sos *sos = (struct marker_sos *)buffer;

	assert(0 == sos->start);
	assert(0x3F == sos->stop);
	assert(0 == sos->select);

	/* only suppprt YCbCr */
	assert(3 == sos->color_component_number);

	// FIX SOS ISSUE, REMOVE 0xFF
	for (int i = 0, j = 0; i<length; i++, j++) {
		buffer[j] = buffer[i];
		//if ((0xFF == buffer[i]) && (0x00 == buffer[i + 1])) {
		if ((0xFF == buffer[i]) && (JPEG_SECTION_SOS != buffer[i + 1])) {
			i++; // FIX ISSUE: FF 00 00 -> FF FF FF
		}
	}

	// TODO more MCUs
	for (int i = 0; i < 64; i++) {
		ret = sos_mcu_parse(sos->data, &bit_offset);
		assert(0 == ret);
	}

	return 0;
}