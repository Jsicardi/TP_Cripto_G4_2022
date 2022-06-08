#ifndef __BMP_FILE_H_
#define __BMP_FILE_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "bmp_header.h"
#include "bmp_info.h"

#define PIXEL_SIZE 24

/* 
    A Pixel is comprised of a single byte for:
        - Blue
        - Green
        - Red
    
    Totals 3 bytes or 24 bits.
    Color range is from 0 to 255.
 */

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} Pixel;

/*
    BMP Files have 54 bytes before its body. Those bytes are divided into:
        - 14 BmpHeader bytes
        - 40 BmpInfo bytes
    
    Checking whether after those 54 bytes one can find the BMP File body or there
    are offset bytes is advised.

    Alternatively, calling read_bmp_file_metadata ensures bytes read afterwards belong to the body.

    read_bmp_file_metadata should be called upon just once per reading instance.
 */

typedef struct {
    BmpHeader header;
    BmpInfo   info;
} BmpFile;

bool read_bmp_file_metadata(BmpFile * bmp_file, FILE * file_descriptor);
bool read_bmp_file_pixel(Pixel * pixel, FILE * file_descriptor);

#endif