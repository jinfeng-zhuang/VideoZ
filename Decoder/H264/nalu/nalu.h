#pragma once

#define NALU_START_CODE_SIZE	4

#define NALU_TYPE_MASK	0x1F

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
    unsigned int offset;
    unsigned int rbsp;
    unsigned int rbsp_len;
    int type;
};
