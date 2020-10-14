#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavutil/error.h>
#include <libavutil/avutil.h>
}

#define INBUF_SIZE 4096

struct frame {
    unsigned char data[3000 * 3000 * 4];
};

static struct frame frames[6];
static int index;

static unsigned char frameDemo[3000 * 3000 * 4];
static unsigned char frameCapture[3000 * 3000 * 4];
static unsigned char rgb[3000 * 3000 * 4];

int display_ready = 0;

static SwsContext* cCtx = NULL;

static void decode(AVCodecContext* dec_ctx, AVFrame* frame, AVPacket* pkt)
{
    int ret;

    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error sending a packet for decoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }

        uint8_t* data[AV_NUM_DATA_POINTERS] = { 0 };
        data[0] = (uint8_t*)rgb;
        int linesize[AV_NUM_DATA_POINTERS] = { dec_ctx->width * 4 };

        // 建议缩放到窗口大小，保持视频比例，就像电视机一样
        int temp_height = 800 * (1.0 * dec_ctx->height / dec_ctx->width);
        cCtx = sws_getCachedContext(cCtx, dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
            800, temp_height, AV_PIX_FMT_BGRA, SWS_BICUBIC,
            NULL, NULL, NULL);

        sws_scale(cCtx, frame->data, frame->linesize, 0, dec_ctx->height, data, linesize);

        while (!display_ready)
            Sleep(10);

        memcpy(frameCapture, rgb, dec_ctx->height * linesize[0]);

        display_ready = 0;
    }
}

int decode_file(const char* filename)
{
    const AVCodec* codec;
    AVCodecParserContext* parser;
    AVCodecContext* c = NULL;
    FILE* f;
    AVFrame* frame;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t* data;
    size_t   data_size;
    int ret;
    AVPacket* pkt;

    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);

    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    /* find the MPEG-1 video decoder */
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    parser = av_parser_init(codec->id);
    if (!parser) {
        fprintf(stderr, "parser not found\n");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    assert(NULL != c);

    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

       /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        printf("Could not open codec\n");
        assert(0);
    }

    f = fopen(filename, "rb");
    if (!f) {
        printf("Could not open %s\n", filename);
        assert(0);
    }

    frame = av_frame_alloc();
    if (!frame) {
        printf("Could not allocate video frame\n");
        assert(0);
    }

    while (!feof(f)) {
        /* read raw data from the input file */
        data_size = fread(inbuf, 1, INBUF_SIZE, f);
        if (!data_size)
            break;

        /* use the parser to split the data into frames */
        data = inbuf;
        while (data_size > 0) {
            ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
                data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
            if (ret < 0) {
                fprintf(stderr, "Error while parsing\n");
                exit(1);
            }
            data += ret;
            data_size -= ret;

            if (pkt->size)
                decode(c, frame, pkt);
        }
    }

    /* flush the decoder */
    decode(c, frame, NULL);

    fclose(f);

    av_parser_close(parser);
    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    return 0;
}

unsigned char* FrameGenerator_Get(void)
{
    display_ready = 1;

    return frameCapture;
}

void FrameGeneratorInit(void)
{
    int framenum;
    int i, j;

    for (i = 0; i < 3000 * 3000 * 4; i += 4) {
        frameDemo[i] = 0xFF;
    }

    for (framenum = 0; framenum < 6; framenum++) {
        for (i = 0; i < 3000; i++) {
            for (j = 0; j < 3000; j++) {
                frames[framenum].data[i * 3000 * 4 + j * 4 + 3] = framenum * 3; // alpha, 0 - transparent, see nothing
                frames[framenum].data[i * 3000 * 4 + j * 4 + 2] = 0xFF;
            }
        }
    }
}

int FrameGenerator(void* arg)
{
    FrameGeneratorInit();

    while (1) {
        decode_file("../Material/test.h264");
        Sleep(3000);
    }
}
