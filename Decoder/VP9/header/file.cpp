#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Local Functions
//-----------------------------------------------------------------------------

static unsigned int file_size(FILE *fp)
{
    unsigned int orig, size;

    orig = ftell(fp);

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    fseek(fp, orig, SEEK_SET);

    return size;
}

//-----------------------------------------------------------------------------
// Global Functions
//-----------------------------------------------------------------------------

unsigned char *file_load(const char *filename, unsigned int *length)
{
    if (NULL == filename)
        return NULL;

    FILE *fp = NULL;
    unsigned int fsize;
    unsigned char *buffer;
    unsigned int bytes_read;

    fopen_s(&fp, filename, "rb");
    if (NULL == fp) {
        printf("file '%s' not found\n", filename);
        goto open_error;
    }

    fsize = file_size(fp);

    buffer = (unsigned char *)malloc(fsize);
    if (NULL == buffer) {
        printf("malloc %d bytes failed\n", fsize);
        goto malloc_error;
    }

    bytes_read = fread(buffer, sizeof(unsigned char), fsize, fp);
    if (bytes_read != fsize) {
        printf("fread, expected %d, but %d\n", fsize, bytes_read);
        goto read_error;
    }

    fclose(fp);

    printf("load file %s done, size %d\n", filename, fsize);

    *length = fsize;

    return buffer;

read_error:
    free(buffer);

malloc_error:
    fclose(fp);

open_error:
    return NULL;
}
