#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getopt.h"
#include "nalu.h"

const char *nalu_type_name[NALU_TYPE_MAX] = {
    "NALU_UNDEF",
    "NALU_SLICE",
    "NALU_SLICE_A",
    "NALU_SLICE_B",
    "NALU_SLICE_C",
    "NALU_SLICE_IDR",
    "NALU_SEI",
    "NALU_SPS",
    "NALU_PPS",
    "NALU_SEQ_END",
    "NALU_SEQ_END2",
    "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF",
    "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF",
    "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF",
    "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF",
    "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF",
    "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF",
    "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF",
    "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF",
    "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF",
    "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF",
    "NALU_UNDEF", "NALU_UNDEF", "NALU_UNDEF"
};

extern unsigned char* load_file(const char* filename, unsigned int *size);

void nalu_type_statistics(struct nalu* nalu_array, unsigned int nalu_count)
{
    unsigned int i;
    int type_count[NALU_TYPE_MAX];

    memset(type_count, 0, sizeof(type_count));

    for (i = 0; i < nalu_count; i++) {
        type_count[nalu_array[i].type]++;
    }

    printf("Type\tName\tCount\n");
    printf("---------------------------------\n");

    for (i = 0; i < NALU_TYPE_MAX; i++) {
        if (0 != type_count[i])
            printf("%d\t%s\t%d\n", i, nalu_type_name[i], type_count[i]);
    }
}

void nalu_type_summary(struct nalu* nalu_array, unsigned int nalu_count)
{
    unsigned int i;

    printf("Offset\tType\tName\t\n");
    printf("---------------------------------\n");

    for (i = 0; i < nalu_count; i++) {
        printf("0x%08x\t%d\t%s\n", nalu_array[i].offset, nalu_array[i].type, nalu_type_name[nalu_array[i].type]);
    }
}

struct nalu* nalu_parse_all(unsigned char* buffer, unsigned int length, unsigned int *count)
{
    unsigned int i;
    bool nalu_start_flag;
    unsigned int nalu_start_offset;
    unsigned int nalu_index;
    unsigned char nalu_header;
    unsigned int nalu_count = 0;

    // Step 1: count of nalu
    for (i = 0; i < length - NALU_START_CODE_SIZE; i++) {
        if ((0 == buffer[i]) && (0 == buffer[i + 1]) && (0 == buffer[i + 2]) && (1 == buffer[i + 3])) {
            nalu_start_flag = true;
            nalu_start_offset = i;
            i += 4;
        }
        else if ((0 == buffer[i]) && (0 == buffer[i + 1]) && (1 == buffer[i + 2])) {
            nalu_start_flag = true;
            nalu_start_offset = i;
            i += 3;
        }
        else {
            nalu_start_flag = false;
            nalu_start_offset = 0;
        }

        if (nalu_start_flag)
            nalu_count++;
    }

    // Step 2: generate nalu_array
    struct nalu* nalu_array = (struct nalu*)malloc(nalu_count * sizeof(struct nalu));
    if (NULL == nalu_array) {
        printf("%s: malloc failed\n", __func__);
        goto END;
    }

    memset(nalu_array, 0, nalu_count * sizeof(struct nalu));

    // Step 3: fill nalu_array
    for (i = 0, nalu_index = 0; i < length - NALU_START_CODE_SIZE; i++) {
        if ((0 == buffer[i]) && (0 == buffer[i + 1]) && (0 == buffer[i + 2]) && (1 == buffer[i + 3])) {
            nalu_array[nalu_index].prefix = 4;
            nalu_array[nalu_index].offset = i + 4;
            nalu_array[nalu_index].buffer = &buffer[i];
            nalu_index++;
            i += 4;
        }
        else if ((0 == buffer[i]) && (0 == buffer[i + 1]) && (1 == buffer[i + 2])) {
            nalu_array[nalu_index].prefix = 3;
            nalu_array[nalu_index].offset = i + 3;
            nalu_array[nalu_index].buffer = &buffer[i];
            nalu_index++;
            i += 3;
        }
        else {
        }
    }

    for (i = 0; i < nalu_count - 1; i++) {
        nalu_array[i].length = nalu_array[i + 1].buffer - nalu_array[i].buffer;
    }
    nalu_array[i].length = &buffer[length] - nalu_array[i].buffer;

    for (i = 0; i < nalu_count; i++) {
        nalu_header = nalu_array[i].buffer[nalu_array[i].prefix];
        //nalu_array[i].type = (enum nal_unit_type)(nalu_header & NALU_TYPE_MASK); // H264
        nalu_array[i].type = (enum nal_unit_type)((nalu_header & 0x7F) >> 1); // H265
    }

END:
    *count = nalu_count;
    return nalu_array;
}

void print_usage(void)
{
    printf(
        "Usage: nalu [OPTION]... [FILE]\n"
        "Analyze nalu header of H264 file\n"
        "\n"
        "  -s\tsummary\n"
        "  -l\tlist\n"
        "  -h\t\tdisplay this help and exit\n"
        "\n"
        "Examples:\n"
        "  nalu -l 0 test.h264\n"
        );
}

int main(int argc, char *argv[])
{
    const char* filename = NULL;
    unsigned int file_size = 0;
    struct nalu* nalu_array = NULL;
    unsigned int nalu_count = 0;
    int opt = 0;
    int select = 0;

    // For DEBUG
    if (1 == argc) {
        argc = 4;
        argv[0] = (char*)"nalu";
        argv[1] = (char*)"-l";
        argv[2] = (char*)"0";
        argv[3] = (char*)"test.h264";
    }

    while ((opt = getopt(argc, argv, "lsh")) != -1) {
        switch (opt) {
        case 's':
            select = 0;
            break;
        case 'l':
            select = 1;
            break;
        default:
            print_usage();
            return -1;
        }
    }

    if (optind >= argc) {
        printf("No input file\n");
        return -1;
    }

    filename = argv[optind];
    
    unsigned char* content = load_file(filename, &file_size);
    if (NULL == content)
        goto END;

    nalu_array = nalu_parse_all(content, file_size, &nalu_count);
    if (NULL == nalu_array)
        goto END;

    if (0 == select)
        nalu_type_statistics(nalu_array, nalu_count);
    else
        nalu_type_summary(nalu_array, nalu_count);

END:
    if (content)
        free(content);

    return 0;
}
