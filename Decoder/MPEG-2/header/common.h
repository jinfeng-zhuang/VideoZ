#ifndef COMMON_H
#define COMMON_H

typedef enum {
	RET_OK = 0,
	RET_FAIL = -1
} return_e;

int load_file(const char *filename, unsigned char **buffer, unsigned int *length);

#endif