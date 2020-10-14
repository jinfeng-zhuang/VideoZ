#include <stdio.h>
#include <assert.h>
#include <string.h>

struct mjpeg_b_header {
    unsigned int unused;
    char tag[4];
    unsigned int field_size;
    unsigned int padded_field_size;
    unsigned int offset_to_next_field;
    unsigned int offset_to_quantization_table;
    unsigned int offset_to_huffman_table;
    unsigned int offset_to_image_data;
    unsigned int offset_to_start_of_scan;
    unsigned int offset_to_start_of_data;
};

unsigned int endian_swap_32(unsigned int val)
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0x00FF00FF);
    return (val << 16) | (val >> 16);
}

#define BUFFER_SIZE (1<<20)

unsigned char buffer[BUFFER_SIZE];

unsigned int buffer_parse(unsigned char* buffer, unsigned int data_size)
{
    unsigned int offset = 0;
    struct mjpeg_b_header header;
    static int bottom_field = 0;
    unsigned int top_field_size = 0;
    unsigned int bottom_field_size = 0;

    // export
    static int frame_count = 0;
    static unsigned int byte_count = 0;
    static unsigned int pts = 0;

    assert(NULL != buffer);

    while (1) {
        if (offset + sizeof(struct mjpeg_b_header) > data_size)
            break;

        memcpy(&header, &buffer[offset], sizeof(struct mjpeg_b_header));

        header.field_size = endian_swap_32(header.field_size);
        header.padded_field_size = endian_swap_32(header.padded_field_size);
        header.offset_to_next_field = endian_swap_32(header.offset_to_next_field);
        header.offset_to_quantization_table = endian_swap_32(header.offset_to_quantization_table);
        header.offset_to_huffman_table = endian_swap_32(header.offset_to_huffman_table);
        header.offset_to_image_data = endian_swap_32(header.offset_to_image_data);
        header.offset_to_start_of_scan = endian_swap_32(header.offset_to_start_of_scan);
        header.offset_to_start_of_data = endian_swap_32(header.offset_to_start_of_data);

        if (offset + header.padded_field_size > data_size)
            break;

        // export ---------------------------------------------
        if (!bottom_field)
            top_field_size = header.padded_field_size;
        else
            bottom_field_size = header.padded_field_size;
        
        if (bottom_field) {
            printf("pts 0x%x size %d\n", pts << 1, top_field_size + bottom_field_size);
            pts += 90 * 1000 / 30; // TODO
        }
        //------------------------------------------------------s

        bottom_field = ~bottom_field;

        offset += header.padded_field_size;
    }

    return offset;
}

int main(int argc, char *argv[])
{
    char* filename;
    FILE* fp;
    int ret;
    unsigned int offset;
    
    unsigned int remain_data;
    unsigned int remain_space;
    
    unsigned int data_size;

#if  1
    argc = 2;
    argv[0] = (char *)"test";
    argv[1] = (char*)"mjpegb.dump";
#endif

    if (2 != argc) {
        printf("Usage: %s [filename]\n", argv[0]);
        return -1;
    }

    filename = argv[1];

    fp = fopen(filename, "rb");
    if (NULL == fp) {
        printf("Error: File not found: \"%s\"\n", filename);
        return -1;
    }

    ret = fread(buffer, 1, BUFFER_SIZE, fp);
    assert(-1 != ret);

    while (1) {

        offset = buffer_parse(buffer, BUFFER_SIZE);

        remain_data = BUFFER_SIZE - offset;
        remain_space = offset;

        memcpy(buffer, &buffer[offset], remain_data);
        ret = fread(&buffer[remain_data], 1, remain_space, fp);
        assert(-1 != ret);

        if (remain_space != ret) {
            data_size = remain_data + ret;
            buffer_parse(buffer, data_size);
            break;
        }
    }

    fclose(fp);

    return 0;
}
