#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned char* load_file(const char* filename, unsigned int* size);
extern int sps_proc(unsigned char* buffer, unsigned int length);

int main(int argc, char* argv[])
{
    unsigned int size = 0;
    unsigned char *raw = NULL;
    unsigned char* buffer = NULL;
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int buflen;

    raw = load_file("sps.bin", &size);

    if ((NULL == raw) || (size == 0)) {
        printf("Error: load sps.bin failed\n");
        goto END;
    }

    buflen = size;
    buffer = (unsigned char*)malloc(buflen);
    if (NULL == buffer)
        goto END;

    memset(buffer, 0, buflen);

    for (i = 0, j = 0; i < size; i++) {
        if ((i >= 2) && (raw[i - 2] == 0) && (raw[i - 1] == 0) && (raw[i] == 3)) {
            buflen--;
            continue;
        }
        else {
            buffer[j] = raw[i];
            j++;
        }
    }

    sps_proc(buffer, buflen);

END:
    if (raw)
        free(raw);
    if (buffer)
        free(buffer);

    return 0;
}
