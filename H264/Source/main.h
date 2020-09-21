#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pic_parameter_set_rbsp.h"
#include "sps.h"

enum nal_unit_type {
	NALU_UNDEF = 0,
	NALU_SLICE = 1,
	NALU_SLICE_A,
	NALU_SLICE_B,
	NALU_SLICE_C,
	NALU_SLICE_IDR,
	NALU_SEI = 6,
	NALU_SPS = 7,
	NALU_PPS = 8,
	NALU_SEQ_END = 9,
	NALU_SEQ_END2 = 10,
	NALU_UNKNOWN,
	NALU_RSVD12,
	NALU_RSVD13,
	NALU_RSVD14,
	NALU_RSVD15,
	NALU_RSVD16,
	NALU_RSVD17,
	NALU_RSVD18,
	NALU_RSVD19,
	NALU_RSVD20,
	NALU_RSVD21,
	NALU_RSVD22,
	NALU_RSVD23,
	NALU_RSVD24,
	NALU_RSVD25,
	NALU_RSVD26,
	NALU_RSVD27,
	NALU_RSVD28,
	NALU_RSVD29,
	NALU_RSVD30,
	NALU_RSVD31,
	NALU_TYPE_MAX = 64
};

struct nalu {
    unsigned int prefix;
    unsigned char *buffer;
    unsigned int length;
	enum nal_unit_type type;
};

struct bitstream_s {
	unsigned int bitpos;
	unsigned char *bits;
};

typedef int (*nalu_callback)(unsigned char *buffer, unsigned int length);

extern nalu_callback g_nalu_callback[];

extern struct bitstream_s bitstream;

// keep nalu in global array, for debug version, as I can dump it
// in release version, nalu_parse should return nalu structure
// return nalu count
extern int nalu_parse_all(unsigned char *content, unsigned int file_size);

extern int nalu_next(enum nal_unit_type *type, unsigned char **buffer, unsigned int *length);

extern int bitstream_init(unsigned char *buffer, unsigned int length);
extern unsigned int u(unsigned int num);
extern unsigned int ue(void);
extern int se(void);
extern int more_rbsp_data(void);
extern int rbsp_trailing_bits(void);
extern int bitstream_error(void);
extern int byte_aligned(void);

extern int sps_proc(unsigned char *buffer, unsigned int length);
extern int pic_pararmeter_set_rbsp_parse(unsigned char* buffer, unsigned int length);
extern int slice_layer_without_partitioning_rbsp_parse(unsigned char* buffer, unsigned int length);

#endif
