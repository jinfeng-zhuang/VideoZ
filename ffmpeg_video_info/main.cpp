#include <stdio.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/mastering_display_metadata.h>
#include <libavutil/dict.h>
#include <libavcodec/avcodec.h>
}

extern int hex_dump(uint8_t* buffer, uint32_t length);

int main(int argc, char** argv)
{
    AVFormatContext* fmt_ctx = NULL;
    AVDictionaryEntry* tag = NULL;
    int ret;
    AVPacket packet;
    int video_stream_index_ = -1;
    char* input_file;
    char* output_file;

    if (argc != 2) {
        printf("usage: %s <input_file>\n"
            "example program to demonstrate the use of the libavformat metadata API.\n"
            "Output: filename, codec info, resolution, number of frames, bitrate of all, bitrate of video\n"
            "\n", argv[0]);
        return -1;
    }

    input_file = argv[1];
    output_file = (char *)"output.txt";

    FILE *output_fd = fopen(output_file, "a");
    if (NULL == output_fd) {
        printf("can't output\n");
        return -2;
    }

    // 如果用第三个参数传入码流信息，最好包含一帧数据，失败的情况下多读一些再调用该函数
    if ((ret = avformat_open_input(&fmt_ctx, input_file, NULL, NULL))) {
        printf("open %s failed\n", input_file);
        return ret;
    }

    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if (ret < 0) {
        printf("avformat_find_stream_info return %d\n", ret);
        return -1;
    }

    //av_dump_format(fmt_ctx, 0, "", 0);

    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]) {
            if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                int64_t bitrate = fmt_ctx->streams[i]->codecpar->bit_rate; // average bitrate
                int width = fmt_ctx->streams[i]->codecpar->width;
                int height = fmt_ctx->streams[i]->codecpar->height;
                enum AVCodecID codec_id = fmt_ctx->streams[i]->codecpar->codec_id;
                int profile = fmt_ctx->streams[i]->codecpar->profile;
                int level = fmt_ctx->streams[i]->codecpar->level;

                const AVCodecDescriptor* codec_desc = avcodec_descriptor_get(codec_id);

                if (codec_desc) {
                    fprintf(output_fd, "\"%s\", %s (Profile %d Level %d), %dx%d, %lld Frames, %lld, %lld\n",
                        input_file,
                        codec_desc->name,
                        profile,
                        level,
                        width, height,
                        fmt_ctx->streams[i]->nb_frames,
                        fmt_ctx->bit_rate>>10,
                        bitrate >> 10
                        );
                }
            }
        }
    }

    avformat_close_input(&fmt_ctx);

    fclose(output_fd);

    return 0;
}
