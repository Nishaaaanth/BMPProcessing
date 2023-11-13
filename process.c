#include <stdio.h>
#include <stdlib.h>
#include "Include/bitmap.h"
#include "Include/dib.h"
#include "Include/image.h"
#include "Include/rgb.h"

struct Image readImage(FILE *bmp_file, int height, int width) {
    struct Image img;

    img.height = height;
    img.width = width;
    img.rgb = (struct RGB **) malloc(height * sizeof(struct RGB *));

    for(int i=height-1; i>=0; i--) {
        img.rgb[i] = (struct RGB *) malloc(width * sizeof(struct RGB));
        fread(img.rgb[i], width, sizeof(struct RGB), bmp_file);
    }

    printf("Image has been read successfully...\n");
    return img;
}

unsigned char greyscale(struct RGB rgb) {
    //ALL THE COEFFICIENTS ARE FROM NTSC (1953)
    const float rCoeff = 0.299f;
    const float gCoeff = 0.587f;
    const float bCoeff = 0.114f;
    const float aCoeff = 0.2126f;
    return ((rgb.red * rCoeff) + (rgb.green * gCoeff) + (rgb.blue * bCoeff) + (rgb.alpha * aCoeff));
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
    FILE *output_file = fopen("./output/output.bmp", "wb");
    if(output_file == NULL) {
        printf("Error outputting the file\n");
        return 1;
    }

    RGBToGreyscale(img);

    fwrite(bmp_header.name, sizeof(bmp_header.name), 1, output_file);
    fwrite(&bmp_header.size, sizeof(bmp_header.size), 1, output_file);
    fwrite(&bmp_header.reserved, sizeof(bmp_header.reserved), 1, output_file);
    fwrite(&bmp_header.offset, sizeof(bmp_header.offset), 1, output_file);
    fwrite(&dib_header, sizeof(struct DIB_header), 1, output_file);

    for(int i=img.height-1; i>=0; i--) {
        fwrite(img.rgb[i], img.width, sizeof(struct RGB), output_file);
    }
    
    fclose(output_file);
    printf("Output has been renedered.\n");
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

    fread(bmp_header.name, sizeof(bmp_header.name), 1, bmp_file);
    fread(&bmp_header.size, sizeof(bmp_header.size), 1, bmp_file);
    fread(&bmp_header.reserved, sizeof(bmp_header.reserved), 1, bmp_file);
    fread(&bmp_header.offset, sizeof(bmp_header.offset), 1, bmp_file);
    
    if((bmp_header.name[0] != 'B') || (bmp_header.name[1] != 'M')) {
        fclose(bmp_file);
        return 1;
    }

    fread(&dib_header, sizeof(struct DIB_header), 1, bmp_file);
    if(dib_header.size != 40 && dib_header.compression != 0 && dib_header.color_depth != 24) {
        fclose(bmp_file);
        printf("File doesn't meet the DIB specification.");
        return 1;
    }
    
    fseek(bmp_file, bmp_header.offset, SEEK_SET);
    struct Image img = readImage(bmp_file, dib_header.height, dib_header.width);
    renderImage(bmp_header, dib_header, img);

    fclose(bmp_file);
    freeImage(img, dib_header.height);
    printf("Image has been modified as output.bmp in \"output\" directory!\n");

    return 0;
}

int main(void) {
    openBMP();

    return 0;
}