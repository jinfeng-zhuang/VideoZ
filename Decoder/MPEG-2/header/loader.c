#include "parser.h"

// input: filename
// output: buffer allocated inside
int load_file(const char *filename, unsigned char **buffer, unsigned int *length)
{
    FILE *f;
    unsigned int filesize;
    unsigned int byteread;
    unsigned char *tmpbuf;
    int ret;

    f = fopen(filename, "rb");
    assert(NULL != f);

    ret = fseek(f, 0, SEEK_END);
    assert(ret == 0);

    filesize = ftell(f);
    assert(filesize >= 0);

    LOG_INFO("file size = %d bytes", filesize);

    fseek(f, 0, SEEK_SET);
    assert(ret == 0);

    tmpbuf = (unsigned char *)malloc(filesize);
    assert(tmpbuf != NULL);

    byteread = fread(tmpbuf, sizeof(unsigned char), filesize, f);
    assert(byteread == filesize);

    fclose(f);

    *buffer = tmpbuf;
    *length = filesize;

    return 0;
}

