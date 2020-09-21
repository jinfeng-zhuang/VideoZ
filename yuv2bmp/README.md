# YUV2RGB

## Usage

Output filename: output_xx.bmp

yuv2bmp [OPTION] [FILE]

    -f yuv format 420sp, 444sp
    -r resolution
    -c count

Example:
    yuv2bmp -f 420sp -r 720x576 -c 10 test.yuv

## YUV Format

* S: Semi
* P: Plannar
* I: Interleave

### YUV420P

* I420: YYYYYYYY UU VV
* YV12: YYYYYYYY VV UU

### YUV420SP

* NV12: YYYYYYYY UVUV
* NV21: YYYYYYYY VUVU
