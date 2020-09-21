#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "getopt.h"

extern int save_bgr_to_bmp(const char* pFileName, unsigned char* pRgbaData, int nWidth, int nHeight);

extern unsigned char I420_Y(int chroma_x, int chroma_y, int stride, int height, unsigned char* frame_buffer);
extern unsigned char I420_U(int chroma_x, int chroma_y, int stride, int height, unsigned char* frame_buffer);
extern unsigned char I420_V(int chroma_x, int chroma_y, int stride, int height, unsigned char* frame_buffer);

#define WIDTH       1920
#define HEIGHT      1080

static unsigned char yuv_buffer[WIDTH * HEIGHT + WIDTH * HEIGHT / 2];

void print_usage(void)
{
    printf(
        "Usage: yuv2bmp [OPTION]... [FILE]\n"
        "Convert YUV format data to BMP file\n"
        "\n"
        "  -f\tyuv Format\n"
        "  -r\tResolution\n"
        "  -c\t\tframe Count\n"
        "\n"
        "Examples:\n"
        "  yuv2bmp -f I420 -r 720x576 -c 10 test.yuv\n"
    );
}

enum format_e {
    FORMAT_NULL,
    FORMAT_I420,
    FORMAT_I444,
};

int main(int argc, char *argv[])
{
    FILE* fp = NULL;
    unsigned char* rgb_buffer = NULL;
    unsigned char* uv_buffer = NULL;
    int i, j;
    unsigned char r, g, b, a;
    unsigned char y, u, v;
    int frame_size;
    int count = 0;
    int frame_count = 0;
    char outfile[32];
    char *inputfile = NULL;
    int opt = 0;
    int width = 0, height = 0;
    enum format_e format = FORMAT_NULL;

    //=========================================================================

    if (1 == argc) {
        argc = 8;
        argv[0] = (char*)"yuvbmp";
        argv[1] = (char*)"-f";
        argv[2] = (char*)"I420";
        argv[3] = (char*)"-r";
        argv[4] = (char*)"1920x1080";
        argv[5] = (char*)"-c";
        argv[6] = (char*)"10";
        argv[7] = (char*)"test.h264.yuv";
    }

    while ((opt = getopt(argc, argv, "f:r:c:")) != -1) {
        switch (opt) {
        case 'f':
            if (0 == strcmp(optarg, "I420")) {
                format = FORMAT_I420;
            }
            else {
                printf("Error: format \'%s\' not supported\n", optarg);
                goto END;
            }
            break;
        case 'r':
            if (2 != sscanf(optarg, "%dx%d", &width, &height)) {
                printf("Error: resolution not valid\n");
                goto END;
            }
            break;
        case 'c':
            frame_count = atoi(optarg);
            if (0 == frame_count) {
                printf("Error: frame count not valid\n");
                goto END;
            }
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

    inputfile = argv[optind];

    if ((NULL == inputfile) || (FORMAT_NULL == format) || (0 == frame_count) || (0 == width) || (0 == height)) {
        print_usage();
        goto END;
    }

    //=========================================================================

    frame_size = width * height + width * height / 2;

    fp = fopen(inputfile, "rb");
    if (NULL == fp) {
        printf("Error: open %s failed\n", inputfile);
        goto END;
    }

    rgb_buffer = (unsigned char*)malloc(width * height * 3);
    if (NULL == rgb_buffer) {
        printf("Error: malloc failed\n");
        goto END;
    }

    //=========================================================================

    for (count = 0; count < frame_count; count++) {
        if (frame_size != fread(yuv_buffer, 1, frame_size, fp))
            goto END;

        for (i = 0; i < HEIGHT; i++) {
            for (j = 0; j < WIDTH; j++) {

                y = I420_Y(j, i, WIDTH, HEIGHT, yuv_buffer);
                u = I420_U(j, i, WIDTH, HEIGHT, yuv_buffer);
                v = I420_V(j, i, WIDTH, HEIGHT, yuv_buffer);

                // YUV TO RGB ================================================
                r = 1.0 * y + 0 + 1.402 * (v - 128);
                g = 1.0 * y - 0.34413 * (u - 128) - 0.71414 * (v - 128);
                b = 1.0 * y + 1.772 * (u - 128) + 0;

                r = (r > 255) ? 255 : r;
                g = (g > 255) ? 255 : g;
                b = (b > 255) ? 255 : b;

                r = (r < 0) ? 0 : r;
                g = (g < 0) ? 0 : g;
                b = (b < 0) ? 0 : b;
                // END =======================================================

                rgb_buffer[(i * WIDTH + j) * 3 + 0] = b;
                rgb_buffer[(i * WIDTH + j) * 3 + 1] = g;
                rgb_buffer[(i * WIDTH + j) * 3 + 2] = r;
            }
        }

        sprintf(outfile, "output_%d.bmp", count);
        save_bgr_to_bmp(outfile, rgb_buffer, WIDTH, HEIGHT);
    }

END:
    if (fp)
        fclose(fp);
    if (rgb_buffer)
        free(rgb_buffer);
}
