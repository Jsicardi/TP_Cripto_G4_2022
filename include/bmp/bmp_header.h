#ifndef __BMP_HEADER_H_
#define __BMP_HEADER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BmpHeader;

/*
    Reads the first 14 bytes which correspond to the header of a
    BMP file V3 (https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader?redirectedfrom=MSDN)
    onto a BmpHeader structure.
 */

bool read_bmp_header(BmpHeader *bmp_header, FILE* file_descriptor);

#endif