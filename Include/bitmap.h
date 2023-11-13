#ifndef BITMAP_HEADER_H
#define BITMAP_HEADER_H

struct BITMAP_header {
    char name[2];
    unsigned int size;
    int reserved;
    unsigned int offset;
};

#endif