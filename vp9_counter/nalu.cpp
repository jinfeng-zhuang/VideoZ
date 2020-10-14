#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "nalu.h"

//=============================================================================
// Local Variables
//=============================================================================

static int nalu_inited;

//=============================================================================
// Global Variables
//=============================================================================

struct nalu *nalu_array;
unsigned int nalu_total;

//=============================================================================
// Local Functions
//=============================================================================

static void nalu_count(unsigned char *buffer, unsigned int length)
{
    unsigned int i;

    nalu_total = 0;

    for (i = 0; i < length; i++) {

        if ((buffer[i + 0] == 0x00) &&
            (buffer[i + 1] == 0x00) &&
            (buffer[i + 2] == 0x01)) {
            i += 3;
            nalu_total++;
        }
        else if ((buffer[i + 0] == 0x00) &&
            (buffer[i + 1] == 0x00) &&
            (buffer[i + 2] == 0x00) &&
            (buffer[i + 3] == 0x01)) {
            i += 4;
            nalu_total++;
        }
    }
}

static void nalu_parse(unsigned char *buffer, unsigned int length)
{
    unsigned int i;
    unsigned int nalu_idx;
    unsigned int nalu_start_flag;

    nalu_idx = 0;

    for (i = 0; i < length;) {
        nalu_start_flag = 0;

        if ((buffer[i + 0] == 0x00) &&
            (buffer[i + 1] == 0x00) &&
            (buffer[i + 2] == 0x01)) {
            i += 3;
            nalu_start_flag = 3;
        }
        else if ((buffer[i + 0] == 0x00) &&
            (buffer[i + 1] == 0x00) &&
            (buffer[i + 2] == 0x00) &&
            (buffer[i + 3] == 0x01)) {
            i += 4;
            nalu_start_flag = 4;
        }
        else {
            i++;
        }

        if (nalu_start_flag) {
            nalu_array[nalu_idx].buffer = &buffer[i];
            nalu_array[nalu_idx].type = (enum nalu_type)((((buffer[i] << 8) | buffer[i + 1]) >> 9) & 0x3F);
            nalu_array[nalu_idx].flag = nalu_start_flag;
            nalu_idx++;
        }
    }

    for (i = 0; i < nalu_total - 1; i++) {
        nalu_array[i].length = nalu_array[i + 1].buffer - nalu_array[i + 1].flag - nalu_array[i].buffer;
    }
    nalu_array[i].length = &buffer[length - 1] - nalu_array[i - 1].buffer;
}

//=============================================================================
// Global Functions
//=============================================================================

int nalu_init(unsigned char *buffer, unsigned int length)
{
    if (NULL == buffer)
        return -1;

    if (NULL != nalu_array) {
        free(nalu_array);
        nalu_array = NULL;
    }

    nalu_inited = 0;

    nalu_count(buffer, length);

    nalu_array = (struct nalu *)malloc(sizeof(struct nalu) * nalu_total);
    if (NULL == nalu_array)
        return -1;

    nalu_parse(buffer, length);

    nalu_inited = 1;

    return nalu_total;
}

int nalu_uninit(void)
{
    if (!nalu_inited)
        return -1;

    free(nalu_array);

    return 0;
}

int nalu_read(unsigned int index, struct nalu *nalu)
{
    if ((index >= nalu_total) || (NULL == nalu))
        return -1;

    if (!nalu_inited)
        return -1;

    memcpy(nalu, &nalu_array[index], sizeof(struct nalu));

    return 0;
}
