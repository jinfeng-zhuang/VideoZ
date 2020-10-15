# VideoZ

* H264 decode
* JPEG decode complete
* ffmpeg decode example
* VP9 and MPEG-2 related code
* ...

## TODO

* 怎么利用 FFMPEG 将 ES流 从容器中提取出来呢
    ffmpeg -i video.mp4 -codec copy -f h264 video.h264

## Project Intro

* y2bmp - 将 Luma Dump 成 BMP 格式， JPEG 图像是直接给 HOST 的， HOST 再交给 DISPLAY，所以从 FRAME BUFFER 中拿不到数据