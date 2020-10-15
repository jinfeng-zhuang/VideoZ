#include <stdio.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
}

int main(int argc, char** argv)
{
    AVFormatContext* fmt_ctx = NULL;
    AVDictionaryEntry* tag = NULL;
    int ret;

    if (argc != 2) {
        printf("usage: %s <input_file>\n"
            "example program to demonstrate the use of the libavformat metadata API.\n"
            "\n", argv[0]);
        return 1;
    }

    // ����õ�������������������Ϣ����ð���һ֡���ݣ�ʧ�ܵ�����¶��һЩ�ٵ��øú���
    if ((ret = avformat_open_input(&fmt_ctx, argv[1], NULL, NULL)))
        return ret;

    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if (ret < 0) {
        printf("avformat_find_stream_info return %d\n", ret);
        return -1;
    }

    av_dump_format(fmt_ctx, 0, "", 0);

    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            break;
        }
    }

    avformat_close_input(&fmt_ctx);

    return 0;
}
