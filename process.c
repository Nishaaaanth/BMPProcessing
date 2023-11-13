#include <stdio.h>
#include <stdlib.h>
#define OFFSET 14

struct BITMAP_header {
    char name[2];
    unsigned int size;
    int reserved;
    unsigned int offset;
};

//BITMAPINFOHEADER: Since this is for Windows NT
struct DIB_header {
    unsigned int size;
    unsigned int width;
    unsigned int height;
    unsigned short int color_plane;
    unsigned short int color_depth;
    unsigned int compression;
    unsigned int image_size;
    // int horizontal_resolution;
    // int vertical_resolution;
    // unsigned int color_palette;
    // unsigned int important_colors;
};

struct RGB {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

struct Image {
    int height;
    int width;
    struct RGB **rgb;
};

struct Image readImage(FILE *bmp_file, int height, int width) {
    struct Image img;

    img.height = height;
    img.width = width;
    img.rgb = (struct RGB **) malloc(height * sizeof(void *));

    for(int i=height-1; i>=0; i--) {
        img.rgb[i] = (struct RGB *) malloc(width * sizeof(struct RGB));
        fread(img.rgb[i], width, sizeof(struct RGB), bmp_file);
    }

    printf("Image has been read successfully...\n");
    return img;
}

//adding & to while passing bmp_file value to readImage
//interchanging width, sizeof(struct RGB) in fread in readImage function
//added * for img in renderImage while passing the value to RBGToGreyscale
//size of  bmp_header is made struct bmp_header instead of 3 * int in renderImage fwrite

unsigned char greyscale(struct RGB rgb) {
    //ALL THE COEFFICIENTS ARE FROM NTSC (1953)
    const float rCoeff = 0.299f;
    const float gCoeff = 0.587f;
    const float bCoeff = 0.114f;
    return ((rgb.red * rCoeff) + (rgb.green * gCoeff) + (rgb.blue * bCoeff));
}

void RGBToGreyscale(struct Image img) {
    if(img.rgb != NULL) {
        for(int i=img.height-1; i>=0; i--) {
            if(img.rgb[i] != NULL) {
                for(int j=img.width-1; j>=0; j--) {
                    unsigned char grey = greyscale(img.rgb[i][j]);
                    img.rgb[i][j].red = img.rgb[i][j].green = img.rgb[i][j].blue = grey;
                }
            }
        }
    }
}

int renderImage(struct BITMAP_header bmp_header, struct DIB_header dib_header, struct Image img) {
    FILE *output_file = fopen("./output/output.bmp", "w");
    if(output_file == NULL) {
        printf("Error outputting the file\n");
        return 1;
    }

    RGBToGreyscale(img);

    fwrite(bmp_header.name, 2, 1, output_file);
    fwrite(&bmp_header.size, 3 * sizeof(int), 1, output_file);
    fwrite(&dib_header, sizeof(struct DIB_header), 1, output_file);

    // fwrite(bmp_header.name, sizeof(bmp_header.name), 1, output_file);
    // fwrite(&bmp_header.size, sizeof(bmp_header.size), 1, output_file);
    // fwrite(&dib_header, sizeof(struct DIB_header), 1, output_file);

    for(int i=img.height-1; i>=0; i--) {
        fwrite(img.rgb[i], img.width, sizeof(struct RGB), output_file);
    }
    
    fclose(output_file);
    printf("Output has been renedered\n");
    return 0;
}

void freeImage(struct Image img, int height) {
    for(int i=0; i<img.height; i++) {
        free(img.rgb[i]);
    }
    free(img.rgb);
}

int openBMP() {
    FILE *bmp_file = fopen("image/image.bmp", "rb");
    if(bmp_file == NULL) {
        printf("Error opening the file.\n");
        return 1;
    }

    printf("File opened successfully...\n");

    struct BITMAP_header bmp_header;
    struct DIB_header dib_header;

    // fread(bmp_header.name, sizeof(bmp_header.name), 1, bmp_file);
    // fread(&bmp_header.size, sizeof(bmp_header.size), 1, bmp_file);
    // fread(&bmp_header.reserved, sizeof(bmp_header.reserved), 1, bmp_file);
    // fread(&bmp_header.offset, sizeof(bmp_header.offset), 1, bmp_file);
    
    fread(bmp_header.name, 2, 1, bmp_file);
    fread(&bmp_header.size, 3 * sizeof(int), 1, bmp_file);

    if((bmp_header.name[0] != 'B') || (bmp_header.name[1] != 'M')) {
        fclose(bmp_file);
        return 1;
    }

    fread(&dib_header, sizeof(struct DIB_header), 1, bmp_file);
    // printf("%s\n%d\n%d\n%d\n", bmp_header.name, bmp_header.size, bmp_header.reserved, bmp_header.offset);
    // printf("%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", dib_header.size, dib_header.width, dib_header.height, dib_header.color_plane, dib_header.color_depth, dib_header.compression, dib_header.horizontal_resolution, dib_header.vertical_resolution, dib_header.image_size, dib_header.color_palette, dib_header.important_colors);

    if(dib_header.size != 40 && dib_header.compression != 0 && dib_header.color_depth != 24) {
        fclose(bmp_file);
        printf("File doesn't meet the DIB specification");
        return 1;
    }
    
    fseek(bmp_file, bmp_header.offset, SEEK_SET);
    struct Image img = readImage(bmp_file, dib_header.height, dib_header.width);
    renderImage(bmp_header, dib_header, img);

    fclose(bmp_file);
    freeImage(img, dib_header.height);
    printf("Image has been modified as output.bmp in \"output\" directory\n");

    return 0;
}

int main(void) {
    openBMP();

    return 0;
}