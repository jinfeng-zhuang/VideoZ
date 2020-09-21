#ifndef TS_H
#define TS_H

#define TS_SYNC_BYTE                0x47

#define TS_PACKET_HEAD_LENGTH       4
#define TS_PACKET_DATA_LENGTH       184
#define TS_PACKET_LENGTH            188

typedef enum {
	PID_PAT = 0,
	PID_NONE = 0x1FFF
} pid_e;

typedef struct ts_packet_header {
	unsigned int sync_byte : 8;
	unsigned int transport_error_indicator : 1;
	unsigned int payload_unit_start_indicator : 1;
	unsigned int transport_priority : 1;
	unsigned int pid : 13;
	unsigned int transport_scrambling_control : 2;
	unsigned int adaptation_field_control : 2;
	unsigned int continue_counter : 4;
} ts_packet_header_t;

int ts_packet_header_get(unsigned char *buffer, ts_packet_header_t *header);
int ts_packet_parse(ts_packet_header_t *header, unsigned char *payload);

#endif