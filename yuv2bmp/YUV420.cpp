unsigned char I420_Y(int chroma_x, int chroma_y, int stride, int height, unsigned char* frame_buffer)
{
    unsigned char value;

    value = frame_buffer[stride * chroma_y + chroma_x];

    return value;
}

unsigned char I420_U(int chroma_x, int chroma_y, int stride, int height, unsigned char* frame_buffer)
{
    int x = chroma_x >> 1;
    int y = chroma_y >> 1;

    unsigned char* buffer = &frame_buffer[stride * height];

    unsigned char value;

    value = buffer[(stride >> 1) * y + x];

    return value;
}

unsigned char I420_V(int chroma_x, int chroma_y, int stride, int height, unsigned char* frame_buffer)
{
    int x = chroma_x >> 1;
    int y = chroma_y >> 1;

    unsigned char* buffer = &frame_buffer[stride * height];

    unsigned char value;

    value = buffer[(stride >> 1) * (height >> 1) + (stride >> 1) * y + x];

    return value;
}