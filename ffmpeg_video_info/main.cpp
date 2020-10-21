#include <stdio.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/mastering_display_metadata.h>
#include <libavutil/dict.h>
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

    argc = 2;
    argv[0] = (char*)"hdr10";
    argv[1] = (char*)"E:/Streams/HDR/HDR10/The World in HDR.mkv";

    if (argc != 2) {
        printf("usage: %s <input_file>\n"
            "example program to demonstrate the use of the libavformat metadata API.\n"
            "\n", argv[0]);
        return 1;
    }

    input_file = argv[1];

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

#if 0
            if (fmt_ctx->streams[i]->codecpar) {
                printf("Stream [%d]: Codec %d\n", i, fmt_ctx->streams[i]->codecpar->codec_type);
            }

            if (fmt_ctx->streams[i]->metadata) {
                printf("Meta Count: %d\n", av_dict_count(fmt_ctx->streams[i]->metadata));

                AVDictionaryEntry* tag = NULL;
                while ((tag = av_dict_get(fmt_ctx->streams[i]->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
                    printf("%s: %s\n", tag->key, tag->value);
                }
            }
#endif

            // refer to 'dump_sidedata'
            if (fmt_ctx->streams[i]->side_data) {
                printf("Slice Data Type: %d, Size: %d\n",
                    fmt_ctx->streams[i]->side_data->type,
                    fmt_ctx->streams[i]->side_data->size);

                // ref to 'dump_mastering_display_metadata'
                if (AV_PKT_DATA_MASTERING_DISPLAY_METADATA == fmt_ctx->streams[i]->side_data->type) {

                    //hex_dump(fmt_ctx->streams[i]->side_data->data, fmt_ctx->streams[i]->side_data->size);

                    AVMasteringDisplayMetadata* metadata = (AVMasteringDisplayMetadata*)(fmt_ctx->streams[i]->side_data->data);
                    printf("\nMastering Display Metadata:\n"
                        "\thas_primaries:%d\n"
                        "\thas_luminance:%d\n"
                        "\tr(%5.4f,%5.4f) g(%5.4f,%5.4f) b(%5.4f %5.4f)\n"
                        "\twp(%5.4f, %5.4f)\n"
                        "\tmin_luminance=%f, max_luminance=%f\n",
                        metadata->has_primaries, metadata->has_luminance,
                        av_q2d(metadata->display_primaries[0][0]),
                        av_q2d(metadata->display_primaries[0][1]),
                        av_q2d(metadata->display_primaries[1][0]),
                        av_q2d(metadata->display_primaries[1][1]),
                        av_q2d(metadata->display_primaries[2][0]),
                        av_q2d(metadata->display_primaries[2][1]),
                        av_q2d(metadata->white_point[0]), av_q2d(metadata->white_point[1]),
                        av_q2d(metadata->min_luminance), av_q2d(metadata->max_luminance));
                }
            }
        }
    }

#if 0
    // ref: xilixili.net/2018/08/20/ffmpeg-got-raw-h264/
    // https://www.jianshu.com/p/21dfd64add77
    while (av_read_frame(fmt_ctx, &packet) >= 0) {
        if (packet.stream_index == video_stream_index_) {
#if 0
            printf("Video Package PTS %llx: %x %x %x %x\n", packet.pts,
                packet.data[0],
                packet.data[1],
                packet.data[2],
                packet.data[3]
            );
#endif
        }
    }
#endif

    avformat_close_input(&fmt_ctx);

    return 0;
}
