#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "parser.h"
#include "debug.h"
#include "bitops.h"

int main(void)
{
    unsigned char *content = NULL;
    unsigned int length;
    int packet_num;
	ts_packet_header_t header;
	unsigned char *buffer;
    int i;

    load_file("example.ts", &content, &length);

    packet_num = length / TS_PACKET_LENGTH;

    LOG_INFO("packet num = %d", packet_num);

	/* process ts layer */
    for (i = 0; i < packet_num; i++) {
		buffer = &content[i * TS_PACKET_LENGTH];

		ts_packet_header_get(buffer, &header);

        ts_packet_parse(&header, &buffer[4]);
    }

    free(content);

    return 0;
}

