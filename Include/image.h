#ifndef IMAGE_H
#define IMAGE_H

struct Image {
    int height;
    int width;
    struct RGB **rgb;
};

#endif