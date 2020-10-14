#include <stdio.h>
#include <stdlib.h>

const char* usage =
"Usage: mjpeg_counter <filename>\n"
"Count SOI marker in MJPEG stream";

int main(int argc, char *argv[])
{
    FILE* fp = NULL;
    char* filename = NULL;
    unsigned int filelen = 0;
    unsigned char* buffer = NULL;
    int count = 0;
    int i;

    if (argc != 2) {
        printf(usage);
        return -1;
    }

    filename = argv[1];

    fp = fopen(filename, "rb");
    if (NULL == fp)
        return -1;

    fseek(fp, 0, SEEK_END);

    filelen = ftell(fp);

    fseek(fp, 0, SEEK_SET);

    buffer = (unsigned char*)malloc(filelen);
    if (NULL == buffer)
        return -1;

    fread(buffer, 1, filelen, fp);

    fclose(fp);

    if (filelen <= 2)
        return -1;

    for (i = 0; i < filelen - 1; i++) {
        if ((buffer[i] == 0xFF) && (buffer[i + 1] == 0xD8))
            count++;
    }

    free(buffer);

    printf("count = %d\n", count);

    return 0;
}
