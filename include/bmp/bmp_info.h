#ifndef __BMP_INFO_H_
#define __BMP_INFO_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define BMP_INFO_SIZE 40

typedef struct {
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BmpInfo;

/*
    Reads the 40 bytes following the BmpHeader. These bytes describe how the pixel
    information is stored in file. Some usefull bits include the 32 bit compression field.

    For more information read: https://gibberlings3.github.io/iesdp/file_formats/ie_formats/bmp.htm
 */

bool read_bmp_info(BmpInfo *bmp_info, FILE* file_descriptor);

/*
    Writes the 40 bytes following the BmpHeader from a BmpInfo structure. These bytes describe how the pixel
    information is stored in file. Some usefull bits include the 32 bit compression field.

    For more information read: https://gibberlings3.github.io/iesdp/file_formats/ie_formats/bmp.htm
 */

bool write_bmp_info(BmpInfo *bmp_info, FILE* file_descriptor);

#endif