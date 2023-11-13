#ifndef DIB_HEADER_H
#define DIB_HEADER_H

//BITMAPINFOHEADER: Since this is for Windows NT
struct DIB_header {
    unsigned int size;
    unsigned int width;
    unsigned int height;
    unsigned short int color_plane;
    unsigned short int color_depth;
    unsigned int compression;
    unsigned int image_size;
    int horizontal_resolution;
    int vertical_resolution;
    unsigned int color_palette;
    unsigned int important_colors;
};

#endif