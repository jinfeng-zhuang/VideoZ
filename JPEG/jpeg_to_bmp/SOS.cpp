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

int huffman_decode_one_by_one(
	uint8_t *bitstream,
	uint32_t *offset,
	struct huffman_tree *tree,
	uint8_t *value
	)
{
	assert(NULL != bitstream);
	assert(NULL != offset);
	assert(NULL != tree);
	assert(NULL != value);

	int ret;
	uint16_t code;
	uint8_t code_width;
	uint8_t symbol;

	code = 0;

	for (int i = 0; i < 16; i++) {
		code = (code << 1) + bitshift(bitstream, offset);
		code_width = i + 1;

		ret = jpeg_huffman_tree_search(tree, code_width, code, &symbol);
		if (0 == ret) {
			*value = symbol;
			return 0;
		}
	}

	printf("[SOS] huffman code width = %d, code = %x\n", code_width, code);

	return -1;
}

// TODO correct the function name
int huffman_and_rle_decode(
	uint8_t *bitstream,
	uint32_t *offset,
	struct huffman_tree *tree,
	uint8_t *huffman_value,
	int *canonical_huffman_value
	)
{
	assert(NULL != bitstream);
	assert(NULL != offset);
	assert(NULL != tree);
	assert(NULL != huffman_value);
	assert(NULL != canonical_huffman_value);

	int ret;
	uint8_t value;
	int zero_count;
	int bits_to_be_read;
	uint16_t huffman_code;
	int dc_ac_value = 0;

	ret = huffman_decode_one_by_one(bitstream, offset, tree, &value);
	assert(0 == ret);

	if (0x00 == value) {
		goto complete;
	}

	zero_count = (value >> 4) & 0xF;

	bits_to_be_read = value & 0xF;
	huffman_code = 0;

	for (int j = 0; j < bits_to_be_read; j++) {
		int bit = bitshift(bitstream, offset);
		huffman_code = (huffman_code << 1) | bit;
	}

	dc_ac_value = jpeg_canonical_huffman_decode(bits_to_be_read, huffman_code);

complete:
	*huffman_value = value;
	*canonical_huffman_value = dc_ac_value;

	return 0;
}

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

int sos_mcu_block_dehuffman(
	uint8_t *src,
	uint32_t *offset,
	struct huffman_tree *dc,
	struct huffman_tree *ac,
	int *prev_dc_value,
	int *dst
	)
{
	assert(NULL != src);
	assert(NULL != dst);
	assert(NULL != offset);
	assert(NULL != dc);
	assert(NULL != ac);

	int ret;
	uint8_t huffman_value;
	int canonical_huffman_value;

	// TAKE ATTENTION
	memset(dst, 0, sizeof(int) * 64);

	// 1 DC coefficient
	ret = huffman_and_rle_decode(src, offset, dc, &huffman_value, &canonical_huffman_value);
	assert(0 == ret);

	if (0x00 == huffman_value) {
		canonical_huffman_value = 0;
	}

	canonical_huffman_value = canonical_huffman_value + *prev_dc_value;
	*prev_dc_value = canonical_huffman_value;
	dst[0] = canonical_huffman_value;

	// 63 AC coefficient
	for (int i = 1; i < 64; i++) {
		ret = huffman_and_rle_decode(src, offset, ac, &huffman_value, &canonical_huffman_value);
		assert(0 == ret);

		if (0x00 == huffman_value) {
			canonical_huffman_value = 0;
			goto complete;
		}

		i = i + ((huffman_value & 0xFF) >> 4);

		// note: matrix init as 0
		dst[i] = canonical_huffman_value;
	}

complete:

	matrix_dump("Original Matrix", dst);

	return 0;
}

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

int YCbCr_to_RGB_211(uint8_t *YCbCr, uint8_t *RGB)
{
	assert(NULL != YCbCr);
	assert(NULL != RGB);

	uint8_t *Y_buf_array[2] = { YCbCr, YCbCr + 64};
	uint8_t *Y_buf;
	uint8_t *Cb_buf = YCbCr + 64 * 2;
	uint8_t *Cr_buf = YCbCr + 64 * 3;

	int C_index;
	int Y_index;

	int Y, Cb, Cr;

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 16; x++) {
			Y_buf = Y_buf_array[x / 8];

			Y_index = (y % 8) * 8 + (x % 8);
			C_index = y * 8 + (x >> 1);

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

int YCbCr_to_RGB_111(uint8_t *YCbCr, uint8_t *RGB)
{
	assert(NULL != YCbCr);
	assert(NULL != RGB);

	uint8_t *Y_buf_array[1] = { YCbCr };
	uint8_t *Y_buf;
	uint8_t *Cb_buf = YCbCr + 64 * 1;
	uint8_t *Cr_buf = YCbCr + 64 * 2;

	int C_index;
	int Y_index;

	int Y, Cb, Cr;

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			Y_buf = YCbCr;

			Y_index = y * 8 + x;
			C_index = y * 8 + x;

			Y = Y_buf[Y_index];
			Cb = Cb_buf[C_index] - 128; // TODO -128
			Cr = Cr_buf[C_index] - 128;

			// SEQ: RGB, BMP SEQ: BGR
			RGB[y * 8 * 3 + x * 3 + 0] = YC2RGB_Value_Convert(Y + ((int)(1.402 * 1024) * Cr) / 1024);
			RGB[y * 8 * 3 + x * 3 + 1] = YC2RGB_Value_Convert(Y - ((int)(0.344 * 1024) * Cb + (int)(0.714 * 1024) * Cr) / 1024);
			RGB[y * 8 * 3 + x * 3 + 2] = YC2RGB_Value_Convert(Y + ((int)(1.772 * 1024) * Cb) / 1024);
		}
	}

	matrix_rgba_dump(RGB);

	output(RGB);

	return 0;
}

static int matrix_dehuffman[64];
static int matrix_dequantization[64];
static uint8_t matrix_idct[6][64];

static uint8_t matrix_rgb[16*16*3];

static int previous_dc_value[3] = { 0, 0, 0 };

int sos_mcu_parse(uint8_t *bitstream, uint32_t *offset, struct jpeg_decoder *info)
{
	int tree_id;
	int quant_id;
	struct huffman_tree *dc_tree;
	struct huffman_tree *ac_tree;
	int *quant_table;

	// YYYY
	tree_id = info->color_to_huffman[0];
	quant_id = info->color_to_quant[0];
	dc_tree = &info->huffman_tree[tree_id][0];
	ac_tree = &info->huffman_tree[tree_id][1];
	if (0 == ac_tree->count) {
		ac_tree = dc_tree;
	}
	quant_table = info->quantization_table[quant_id];

	for (int i = 0; i < info->Y_number; i++) {
		sos_mcu_block_dehuffman(bitstream, offset, dc_tree, ac_tree, &previous_dc_value[0], matrix_dehuffman);
		jpeg_inverse_quantization(matrix_dehuffman, matrix_dequantization, quant_table);
		jpeg_block_idct(matrix_dequantization, matrix_idct[i]);
	}

	//CB
	tree_id = info->color_to_huffman[1];
	quant_id = info->color_to_quant[1];
	dc_tree = &info->huffman_tree[tree_id][0];
	assert(0 != dc_tree->count);
	ac_tree = &info->huffman_tree[tree_id][1];
	assert(0 != ac_tree->count);
	quant_table = info->quantization_table[quant_id];

	sos_mcu_block_dehuffman(bitstream, offset, dc_tree, ac_tree, &previous_dc_value[1], matrix_dehuffman);
	jpeg_inverse_quantization(matrix_dehuffman, matrix_dequantization, quant_table);
	jpeg_block_idct(matrix_dequantization, matrix_idct[info->Y_number]);

	//CR
	tree_id = info->color_to_huffman[2];
	quant_id = info->color_to_quant[2];
	dc_tree = &info->huffman_tree[tree_id][0];
	assert(0 != dc_tree->count);
	ac_tree = &info->huffman_tree[tree_id][1];
	assert(0 != ac_tree->count);
	quant_table = info->quantization_table[quant_id];

	sos_mcu_block_dehuffman(bitstream, offset, dc_tree, ac_tree, &previous_dc_value[2], matrix_dehuffman);
	jpeg_inverse_quantization(matrix_dehuffman, matrix_dequantization, quant_table);
	jpeg_block_idct(matrix_dequantization, matrix_idct[info->Y_number+1]);

	if (4 == info->Y_number) {
		YCbCr_to_RGB_411((uint8_t *)matrix_idct, matrix_rgb);
	}
	else if (2 == info->Y_number) {
		YCbCr_to_RGB_211((uint8_t *)matrix_idct, matrix_rgb);
	}
	else if (1 == info->Y_number) {
		YCbCr_to_RGB_111((uint8_t *)matrix_idct, matrix_rgb);
	}
	else {
		assert(0);
	}

	return 0;
}

int jpeg_sos_decode(uint8_t *buffer, uint32_t length, struct jpeg_decoder *info)
{
	assert(NULL != buffer);

	int ret;
	uint32_t bit_offset = 0;
	int mcu_number_horizontal;
	int mcu_number_vertical;
	
	printf("[SOS] section length = %d\n", length);

	struct marker_sos *sos = (struct marker_sos *)buffer;

	assert(0 == sos->start);
	assert(0x3F == sos->stop);
	assert(0 == sos->select);
	assert(3 == sos->color_component_number);
	assert(12 == BIGENDIAN_16(sos->length));

	assert((0 == sos->color_component[0].id) || (1 == sos->color_component[0].id));
	assert(0 == sos->color_component[0].huffman_table_ac_id);
	assert(0 == sos->color_component[0].huffman_table_dc_id);

	assert((1 == sos->color_component[1].id) || (2 == sos->color_component[1].id));
	assert(1 == sos->color_component[1].huffman_table_ac_id);
	assert(1 == sos->color_component[1].huffman_table_dc_id);

	assert((2 == sos->color_component[2].id) || (3 == sos->color_component[2].id));
	assert(1 == sos->color_component[2].huffman_table_ac_id);
	assert(1 == sos->color_component[2].huffman_table_dc_id);

	assert(sos->color_component[0].huffman_table_dc_id == sos->color_component[0].huffman_table_ac_id);
	assert(sos->color_component[1].huffman_table_dc_id == sos->color_component[1].huffman_table_ac_id);
	assert(sos->color_component[2].huffman_table_dc_id == sos->color_component[2].huffman_table_ac_id);

	info->color_to_huffman[0] = sos->color_component[0].huffman_table_dc_id;
	info->color_to_huffman[1] = sos->color_component[1].huffman_table_dc_id;
	info->color_to_huffman[2] = sos->color_component[2].huffman_table_dc_id;

	mcu_number_horizontal = (info->width / info->mcu_width) + (info->width % info->mcu_width ? 1 : 0);
	mcu_number_vertical = (info->height / info->mcu_height) + (info->height % info->mcu_height ? 1 : 0);

	for (uint32_t i = 0, j = 0; i<length; i++, j++) {
		buffer[j] = buffer[i];
		if ((0xFF == buffer[i]) && (JPEG_SECTION_SOS != buffer[i + 1])) {
			i++; // TODO SKIP following 0, WHY
		}
	}

	// TODO more MCUs
	for (int i = 0; i < mcu_number_horizontal * mcu_number_vertical; i++) {
		ret = sos_mcu_parse(sos->data, &bit_offset, info);
		assert(0 == ret);
	}

	return 0;
}