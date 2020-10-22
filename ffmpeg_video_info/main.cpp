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
            "\n", argv[0]);
        return -1;
    }

    input_file = argv[1];
    output_file = (char *)"output.txt";

    FILE *output_fd = fopen(output_file, "a+");
    if (NULL == output_fd) {
        printf("can't output\n");
        return -2;
    }

    // 检查如果是空文件，添加头部信息
    fseek(output_fd, 0, SEEK_END);

    if (0 == ftell(output_fd)) {
        fprintf(output_fd, "Filename, Type, Codec, Profile, Level, Resolution/Sample, Framerate, Bitrate (Kbps)\n");
    }

    fseek(output_fd, 0, SEEK_SET);

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

    // 参考用
    //av_dump_format(fmt_ctx, 0, "", 0);

    // 格式化输出
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]) {
            if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                int64_t bitrate = fmt_ctx->streams[i]->codecpar->bit_rate; // average bitrate // not used
                int width = fmt_ctx->streams[i]->codecpar->width;
                int height = fmt_ctx->streams[i]->codecpar->height;
                enum AVCodecID codec_id = fmt_ctx->streams[i]->codecpar->codec_id;
                int level = fmt_ctx->streams[i]->codecpar->level;
                float rate = av_q2d(fmt_ctx->streams[i]->avg_frame_rate);
                const char* profile = avcodec_profile_name(codec_id, fmt_ctx->streams[i]->codecpar->profile);

                const AVCodecDescriptor* codec_desc = avcodec_descriptor_get(codec_id);

                if (codec_desc) {
                    fprintf(output_fd, "\"%s\", Video, %s, %s, %d, %dx%d, %f, %lld\n",
                        input_file,
                        codec_desc->name,
                        profile,
                        level,
                        width, height,
                        rate,
                        fmt_ctx->bit_rate>>10
                        );
                }
            }
            else if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                enum AVCodecID codec_id = fmt_ctx->streams[i]->codecpar->codec_id;
                const AVCodecDescriptor* codec_desc = avcodec_descriptor_get(codec_id);
                const char* profile = avcodec_profile_name(codec_id, fmt_ctx->streams[i]->codecpar->profile);
                int level = fmt_ctx->streams[i]->codecpar->level;
                float rate = av_q2d(fmt_ctx->streams[i]->avg_frame_rate);
                int sample_rate = fmt_ctx->streams[i]->codecpar->sample_rate;
                int64_t bitrate = fmt_ctx->streams[i]->codecpar->bit_rate;

                if (codec_desc) {
                    fprintf(output_fd, "\"%s\", Audio, %s, %s, %d, %d, , %lld\n",
                        input_file,
                        codec_desc->name,
                        profile,
                        level,
                        sample_rate,
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
