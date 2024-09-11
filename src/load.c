#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "globals.c"

#pragma pack(push, 1)
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} BMPHeader;

typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPixelsPerMeter;
    int32_t yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
} BMPInfoHeader;
#pragma pack(pop)

typedef struct {
    uint32_t *buffer;
    int width;
    int height;
} sprite_atlas;

sprite_atlas load_image_bmp(const char *filename) {
    // initialize sprite
    sprite_atlas image;

    // open file
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open image file: %s\n", filename);
        exit(1);
    }

    // read header
    BMPHeader header;
    BMPInfoHeader infoHeader;
    fread(&header, sizeof(BMPHeader), 1, file);
    fread(&infoHeader, sizeof(BMPInfoHeader), 1, file);
    if (header.type != 0x4D42) { // "BM" in little endian
        fprintf(stderr, "Not a valid BMP file\n");
        fclose(file);
        exit(1);
    }
    if (infoHeader.bitsPerPixel != 24 && infoHeader.bitsPerPixel != 32) {
        fprintf(stderr, "Only 24-bit and 32-bit BMP files are supported\n");
        fclose(file);
        exit(1);
    }

    image.width = infoHeader.width;
    image.height = abs(infoHeader.height); // Handle top-down BMPs

    int stride = ((image.width * infoHeader.bitsPerPixel + 31) / 32) * 4;

    // allocate memory for image buffer
    image.buffer = (uint32_t *)malloc((image.width) * (image.height) * sizeof(uint32_t));
    if (!image.buffer) {
        fprintf(stderr, "Failed to allocate memory for image buffer\n");
        fclose(file);
        exit(1);
    }

    fseek(file, header.offset, SEEK_SET); // move to start of pixel data

    // read pixel data
    for (int y = 0; y < image.height; y++) { // for each row
        int row = (infoHeader.height < 0) ? y : (image.height - 1 - y); // if height is negative, the image is top-down, otherwise bottom-up
        for (int x = 0; x < image.width; x++) { // for each pixel in the row
            uint8_t pixel[4] = {0, 0, 0, 255}; // initialize pixel to transparent black
            fread(pixel, infoHeader.bitsPerPixel / 8, 1, file); // read pixel data from file
            (image.buffer)[row * (image.width) + x] = (pixel[3] << 24) | (pixel[2] << 16) | (pixel[1] << 8) | pixel[0];
        }
        fseek(file, stride - (image.width * infoHeader.bitsPerPixel / 8), SEEK_CUR); // move to the next row
    }

    fclose(file);
    return image;
}

void test() {
    printf("test\n");
}
