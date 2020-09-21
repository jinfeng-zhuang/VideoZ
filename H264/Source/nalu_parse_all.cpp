#include "main.h"

#define NALU_START_CODE_SIZE	4

#define NALU_TYPE_MASK	0x1F

struct nalu *nalu_array;
unsigned int nalu_count;

void nalu_type_statistics(void)
{
	unsigned int i;
	int type_count[NALU_TYPE_MAX];

	memset(type_count, 0, sizeof(type_count));
	
	for (i = 0; i < nalu_count; i++) {
		type_count[nalu_array[i].type]++;
	}

	for (i = 0; i < NALU_TYPE_MAX; i++) {
		if (0 != type_count[i])
			printf("type[%d] count = %d\n", i, type_count[i]);
	}
}

int nalu_parse_all(unsigned char *buffer, unsigned int length)
{
	unsigned int i;
	bool nalu_start_flag;
	unsigned int nalu_start_offset;
	unsigned int nalu_index;
	unsigned char nalu_header;

	nalu_count = 0;

	for (i = 0; i < length - NALU_START_CODE_SIZE; i++) {
		if ((0 == buffer[i]) && (0 == buffer[i + 1]) && (0 == buffer[i + 2]) && (1 == buffer[i + 3])) {
			nalu_start_flag = true;
			nalu_start_offset = i;
			i += 4;
		}
		else if ((0 == buffer[i]) && (0 == buffer[i + 1]) && (1 == buffer[i + 2])) {
			nalu_start_flag = true;
			nalu_start_offset = i;
			i += 3;
		}
		else {
			nalu_start_flag = false;
			nalu_start_offset = 0;
		}

		if (nalu_start_flag)
			nalu_count++;
	}

	printf("nalu count %d\n", nalu_count);

	nalu_array = (struct nalu *)malloc(nalu_count * sizeof(struct nalu));
	if (NULL == nalu_array) {
		printf("no enough space for nalu\n");
		goto no_enough_space;
	}

	memset(nalu_array, 0, nalu_count * sizeof(struct nalu));

	nalu_index = 0;

	for (i = 0; i < length - NALU_START_CODE_SIZE; i++) {
		if ((0 == buffer[i]) && (0 == buffer[i + 1]) && (0 == buffer[i + 2]) && (1 == buffer[i + 3])) {
			nalu_array[nalu_index].prefix = 4;
			nalu_array[nalu_index].buffer = &buffer[i];
			nalu_index++;
			i += 4;
		}
		else if ((0 == buffer[i]) && (0 == buffer[i + 1]) && (1 == buffer[i + 2])) {
			nalu_array[nalu_index].prefix = 3;
			nalu_array[nalu_index].buffer = &buffer[i];
			nalu_index++;
			i += 3;
		}
		else {
		}
	}

	for (i = 0; i < nalu_count - 1; i++) {
		nalu_array[i].length = nalu_array[i + 1].buffer - nalu_array[i].buffer;
	}
	nalu_array[i].length = &buffer[length] - nalu_array[i].buffer;

	for (i = 0; i < nalu_count; i++) {
		nalu_header = nalu_array[i].buffer[nalu_array[i].prefix];
		nalu_array[i].type = (enum nal_unit_type)(nalu_header & NALU_TYPE_MASK);
        //nalu_array[i].type = (enum nal_unit_type)((nalu_header & 0x7F) >> 1); // H265
	}

	nalu_type_statistics();

	return nalu_count;

no_enough_space:
	return -1;
}

int nalu_next(enum nal_unit_type *type, unsigned char **buffer, unsigned int *length)
{
	static unsigned int index = 0;
	static unsigned char* payload = NULL;

	unsigned char *tmpbuf;
	unsigned int tmplen;
	enum nal_unit_type tmptype;

	unsigned int i, j;

	if (index >= nalu_count)
		return -1;

	if (payload)
		free(payload);

	tmptype = nalu_array[index].type;
	tmplen = nalu_array[index].length;
	tmpbuf = &nalu_array[index].buffer[nalu_array[index].prefix + 1]; // skip header

	payload = (unsigned char*)malloc(tmplen);
	if (NULL == payload) {
		printf("no enough space\n");
		return -1;
	}

	for (i = 0, j = 0; i < tmplen; i++) {
		if ((i >= 2) && (0 == tmpbuf[i - 2]) && (0 == tmpbuf[i - 1]) && (3 == tmpbuf[i])) {
			tmplen--;
			continue;
		}
		payload[j] = tmpbuf[i];
		j++;
	}

	*type = tmptype;
	*buffer = payload;
	*length = tmplen;

	index++;

	return 0;
}
