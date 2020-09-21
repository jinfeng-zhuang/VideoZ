# Overview

* JPEG: markers
* MJPEG-A: markers
* MJPEG-B: header replace markers, the header = app 1 marker in MJPEG-A

# Header

    unused
    tag
    field size
    padded field size
    offset to next field
    quantization table offset
    huffman table offset
    start of image offset
    start of scan offset
    start of data offset

# Reference

* QuickTime-JPEGSpec.pdf