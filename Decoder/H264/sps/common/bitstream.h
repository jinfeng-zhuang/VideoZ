#pragma once

struct bitstream_s {
    unsigned int bitpos;
    unsigned char* bits;
};

extern int bitstream_init(unsigned char* buffer, unsigned int length);
extern unsigned int u(unsigned int num);
extern unsigned int ue(void);
extern int se(void);
extern int more_rbsp_data(void);
extern int rbsp_trailing_bits(void);
extern int bitstream_error(void);
extern int byte_aligned(void);

extern struct bitstream_s bitstream;
