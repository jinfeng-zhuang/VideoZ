#ifndef PAT_H
#define PAT_H

struct pat_program {
	unsigned int program_num;
	unsigned int program_pid;
};

struct pat {
	unsigned int table_id;	/* 00 - PAT */
	unsigned int section_syntax_indicator;
	unsigned int zero;
	unsigned int rsvd1;
	unsigned int section_length;
	unsigned int transport_stream_id;
	unsigned int rsvd2;
	unsigned int version_number; /* PAT version */
	unsigned int current_next_indicator; /* ? */
	unsigned int section_number;
	unsigned int last_section_number;
	unsigned int rsvd3;
	unsigned int network_pid;
	unsigned int crc_32;
	struct pat_program program[1];
};

int ts_pat_parse(unsigned char *payload);

#endif