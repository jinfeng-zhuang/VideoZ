//
// ref to https://www.cnblogs.com/shakin/p/3714848.html
//

#include "parser.h"

static struct pat pat;

static void pat_dump(struct pat* pat)
{
    printf(
        "--------------------------------------------------\n"
        "                      PAT                         \n"
        "--------------------------------------------------\n"
        "table_id: %d\n"
        "section_syntax_indicator: %d\n"
        "section_length: %d\n"
        "transport_stream_id: %d\n"
        "version_number: %d\n"
        "current_next_indicator: %d\n"
        "section_number: %d\n"
        "last_section_number: %d\n"
        "network_pid: %d\n"
        "crc_32: %x\n",

        pat->table_id,
        pat->section_syntax_indicator,
        pat->section_length,
        pat->transport_stream_id,
        pat->version_number,
        pat->current_next_indicator,
        pat->section_number,
        pat->last_section_number,
        pat->network_pid,
        pat->crc_32
    );
}

int ts_pat_parse(unsigned char *buf)
{
	int len;
	int i;
	unsigned char *prog_buf;
	unsigned int prog_num;
	unsigned int prog_pid;
	unsigned int prog_inc;

	pat.table_id = buf[0];
	
	pat.section_syntax_indicator = BITGET(buf[1], 7, 7);
	pat.zero = BITGET(buf[1], 6, 6);
	pat.rsvd1 = BITGET(buf[1], 5, 4);
	
	pat.section_length = BITGET(buf[1], 3, 0) << 8 | buf[2];

	pat.transport_stream_id = buf[3] << 8 | buf[4];

	pat.rsvd2 = BITGET(buf[5], 7, 6);
	pat.version_number = BITGET(buf[5], 5, 1);
	pat.current_next_indicator = BITGET(buf[5], 0, 0);

	pat.section_number = buf[6];
	pat.last_section_number = buf[7];

	len = pat.section_length + 3; /* len  = pos of section_length (offset 3) to crc */
	pat.crc_32 = buf[len - 4] << 24 | buf[len - 3] << 16 | buf[len - 2] << 8 | buf[len - 1];

	len = pat.section_length - 12; /* remove crc and header before it */
	prog_buf = &buf[8];
	prog_inc = 0;
	for (i = 0; i < len; i += 4) { /* 4 is the structure size */
		pat.rsvd3 = BITGET(prog_buf[i + 2], 7, 5);
		
		prog_num = (prog_buf[i] << 8) | prog_buf[i + 1];
		prog_pid = BITGET(prog_buf[i + 2], 4, 0) << 8 | prog_buf[i + 3];

		pat.network_pid = (0x00 != prog_num) ? 0 : prog_pid; /* check every program ? */

		if (0x00 != prog_num) {
			pat.program[prog_inc].program_num = prog_num;
			pat.program[prog_inc].program_pid = prog_pid;
			prog_inc++;
		}
	}

    pat_dump(&pat);

	return 0;
}