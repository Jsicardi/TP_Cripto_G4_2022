#ifndef __BMP_HEADER_H_
#define __BMP_HEADER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define BMP_HEADER_SIZE 14

typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BmpHeader;

/*
    Sets all BmpHeader fields to 0 to prevent leaking information into memory
    and becoming a potential security vulnerability.
 */

void clean_bmp_header(BmpHeader *bmp_header);

/*
    Reads the first 14 bytes which correspond to the header of a
    BMP file V3 (https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader?redirectedfrom=MSDN)
    onto a BmpHeader structure.
 */

bool read_bmp_header(BmpHeader *bmp_header, FILE* file_descriptor);

/*
    Writes the first 14 bytes which correspond to the header of a
    BMP file V3 (https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader?redirectedfrom=MSDN)
    from a BmpHeader structure onto a file_descriptor.
 */

bool write_bmp_header(BmpHeader *bmp_header, FILE* file_descriptor);

/* 
    Sets pixel_count to be the file size - the metadata offset over 3 resulting in the total number of pixels 
 */

void get_bmp_file_body_pixel_count(uint32_t * pixel_count, BmpHeader * bmp_header);

#endif