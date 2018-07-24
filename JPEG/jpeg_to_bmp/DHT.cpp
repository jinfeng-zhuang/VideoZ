#include "jpeg_section.h"

static int huffman_code_to_bitstream(int bitwidth, uint16_t code, char *bitstream)
{
	assert(NULL != bitstream);

	int i;

	for (i = 0; i < bitwidth; i++) {
		bitstream[i] = (code & (1 << (bitwidth - 1 - i))) ? '1' : '0';
	}

	bitstream[i] = '\0';

	return 0;
}

static int huffman_table_dump(struct huffman_tree *tree)
{
	assert(NULL != tree);

	printf("------------------------------------------\n");
	printf("SYMBOL\tHuffmanCode\n");
	for (int i = 0; i < tree->count; i++) {
		printf("%02x\t\t\t%s\n", tree->symbol[i], tree->bitstream[i]);
	}
	printf("------------------------------------------\n");

	return 0;
}

int jpeg_dht_decode(uint8_t *buffer, uint32_t length, struct jpeg_decoder *info)
{
	assert(NULL != buffer);
	assert(NULL != info);

	struct jpeg_marker_dht *dht = (struct jpeg_marker_dht *)buffer;

	uint32_t symbol_count = 0;
	uint16_t symbol_current_value = 0;
	int bitwidth;
	uint32_t huffman_symbol_number = 0;

	assert(BIGENDIAN_16(dht->length) + 2 == length);

	printf("Table ID: %d, DC/AC : %d\n", dht->id, dht->ac_dc);

	for (int i=0; i<16; i++) {
		huffman_symbol_number += dht->number_of_level[i];
	}

	info->huffman_tree[dht->id][dht->ac_dc].count = huffman_symbol_number;

	for (int i=0; i<16; i++) {
		for (int j=0; j<dht->number_of_level[i]; j++) {

			bitwidth = i + 1; /* start from 1 bit */

			info->huffman_tree[dht->id][dht->ac_dc].bitwidth[symbol_count] = bitwidth;
			info->huffman_tree[dht->id][dht->ac_dc].symbol[symbol_count] = dht->data[symbol_count];
			info->huffman_tree[dht->id][dht->ac_dc].code[symbol_count] = symbol_current_value;

			huffman_code_to_bitstream(bitwidth, symbol_current_value, info->huffman_tree[dht->id][dht->ac_dc].bitstream[symbol_count]);

			symbol_count++;
			symbol_current_value++;
		}

		symbol_current_value = (symbol_current_value) << 1;
	}

	huffman_table_dump(&info->huffman_tree[dht->id][dht->ac_dc]);

	return 0;
}

// TODO OPTIMIZE BY BIT WIDTH
int jpeg_huffman_tree_search(
	struct huffman_tree *tree,
	int huffman_code_width,
	uint16_t huffman_code,
	uint8_t *symbol
	)
{
	assert(NULL != tree);
	assert(NULL != symbol);

	int ret;
	char bitstream[17];

	for (int i = 0; i < tree->count; i++) {
		if ((huffman_code == tree->code[i]) && (huffman_code_width == tree->bitwidth[i])) {
			*symbol = tree->symbol[i];
			printf("%s (%02xH) ", tree->bitstream[i], *symbol);
			return 0;
		}
	}

	return -1;
}

int jpeg_canonical_huffman_decode(uint8_t bit_width, uint16_t huffman_code)
{
	int value;

	uint16_t mask_flag = 1 << (bit_width - 1);

	int flag = (huffman_code & mask_flag) ? 1 : 0;
	if (0 == flag) {
		value = (~huffman_code) & ((1 << bit_width) - 1);
		value = value * -1;
	}
	else {
		value = huffman_code;
	}

	// DEBUG
	char bitstream[17];
	huffman_code_to_bitstream(bit_width, huffman_code, bitstream);
	printf("%s [%d] ", bitstream, value);

	return value;
}