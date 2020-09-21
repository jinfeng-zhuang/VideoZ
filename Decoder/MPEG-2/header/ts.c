#include "parser.h"

void ts_packet_head_dump(ts_packet_header_t *header)
{
	printf("\nTS Packet\n"
		"=================================\n"
		"transport_error_indicator    = %d\n"
		"payload_unit_start_indicator = %d\n"
		"transport_priority           = %d\n"
		"pid                          = %d\n"
		"transport_scrambling_control = %d\n"
		"adaptation_field_control     = %d\n"
		"continue_counter             = %d\n",
		header->transport_error_indicator,
		header->payload_unit_start_indicator,
		header->transport_priority,
		header->pid,
		header->transport_scrambling_control,
		header->adaptation_field_control,
		header->continue_counter
	);
}

int ts_packet_header_get(unsigned char *buffer, ts_packet_header_t *header)
{
	header->sync_byte = buffer[0];
	header->transport_error_indicator = BITGET(buffer[1], 7, 7);
	header->payload_unit_start_indicator = BITGET(buffer[1], 6, 6);
	header->transport_priority = BITGET(buffer[1], 5, 5);
	header->pid = BITGET(buffer[1], 4, 0) << 8 | buffer[2];
	header->transport_scrambling_control = BITGET(buffer[3], 7, 6);
	header->adaptation_field_control = BITGET(buffer[3], 5, 4);
	header->continue_counter = BITGET(buffer[3], 3, 0);

	return 0;
}

int ts_packet_parse(ts_packet_header_t *header, unsigned char *payload)
{
	assert(header->sync_byte == TS_SYNC_BYTE);

	if (header->pid == PID_PAT) {
		ts_pat_parse(payload);
	}

	return 0;
}