#include "jpeg_section.h"

struct huffman_db huffman_table = {
	4,
	{ 0x00, 0x01, 0x10, 0x11 },
	{ NULL, NULL, NULL, NULL }
};

static int huffman_code_to_bitstream(int bitwidth, uint16_t code, uint8_t *bitstream)
{
	assert(NULL != bitstream);

	int i;

	for (i = 0; i < bitwidth; i++) {
		bitstream[i] = (code & (1 << (bitwidth - 1 - i))) ? '1' : '0';
	}

	bitstream[i] = '\0';

	return 0;
}

static int huffman_table_dump(struct huffman_db *db, uint8_t index)
{
	assert(NULL != db);

	int real_index;

	for (int i = 0; i < db->number; i++) {
		if (db->index[i] == index) {
			real_index = i;
		}
	}

	printf("------------------------------------------\n");
	printf("SYMBOL\t Bit(s)\t HuffmanCode\n");
	for (int j = 0; j < db->symbol_number[real_index]; j++) {
		printf("%02x\t %d\t %s\n", db->symbol[real_index][j].symbol, db->symbol[real_index][j].bit_width, db->symbol[real_index][j].huffman_bitstream);
	}
	printf("------------------------------------------\n");

	return 0;
}

int jpeg_dht_decode(uint8_t *buffer, uint32_t length, struct huffman_db *db)
{
	assert(NULL != buffer);
	assert(0 != length);
	assert(NULL != db);

	int i;

	// INPUT
	struct jpeg_marker_dht *dht = (struct jpeg_marker_dht *)buffer;

	// INTERNAL
	uint32_t symbol_count = 0;
	uint16_t symbol_current_value = 0;
	int bitwidth;

	// OUTPUT
	struct huffman_symbol *huffman_symbol_table;
	uint32_t huffman_symbol_number = 0;

	printf("Table Index: %02x\n", dht->huffman_table_index);

	for (int i=0; i<16; i++) {
		huffman_symbol_number += dht->number_of_level[i];
	}

	huffman_symbol_table = (struct huffman_symbol *)malloc(sizeof(struct huffman_symbol) * huffman_symbol_number);
	assert(NULL != huffman_symbol_table);

	// 16 bit width levels
	for (int i=0; i<16; i++) {
		for (int j=0; j<dht->number_of_level[i]; j++) {

			bitwidth = i + 1; /* start from 1 bit */

			huffman_symbol_table[symbol_count].symbol = dht->data[symbol_count];
			huffman_symbol_table[symbol_count].bit_width = bitwidth;
			huffman_symbol_table[symbol_count].huffman_code = symbol_current_value;

			huffman_code_to_bitstream(bitwidth, symbol_current_value, huffman_symbol_table[symbol_count].huffman_bitstream);

			symbol_count++;
			symbol_current_value++;
		}

		symbol_current_value = (symbol_current_value) << 1;
	}

	for (i = 0; i < db->number; i++) {
		if (db->index[i] == dht->huffman_table_index) {
			db->symbol_number[i] = huffman_symbol_number;
			db->symbol[i] = huffman_symbol_table;
			break;
		}
	}

	assert(db->number != i);

	huffman_table_dump(db, dht->huffman_table_index);

	return 0;
}

int jpeg_huffman_tree_search(struct huffman_db *db, uint8_t index, int huffman_code_width, uint16_t huffman_code, uint8_t *symbol)
{
	assert(NULL != db);
	assert(NULL != symbol);

	int i;
	int real_index;

	for (i = 0; i < db->number; i++) {
		if (db->index[i] == index) {
			real_index = i;
		}
	}

	for (i = 0; i < db->symbol_number[real_index]; i++) {
		if ((huffman_code_width == db->symbol[real_index][i].bit_width) && (huffman_code == db->symbol[real_index][i].huffman_code)) {
			goto end;
		}
	}

	return -1;

end:
	printf("%s (%02xH) ", 
		db->symbol[real_index][i].huffman_bitstream,
		db->symbol[real_index][i].symbol);

	*symbol = db->symbol[real_index][i].symbol;
	return 0;
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
	uint8_t bitstream[17];
	huffman_code_to_bitstream(bit_width, huffman_code, bitstream);
	printf("%s [%d] ", bitstream, value);

	return value;
}