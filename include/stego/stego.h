#ifndef __STEGO_H_
#define __STEGO_H_

#include "../bmp/bmp_file.h"
#include "../utils/binary_utils.h"

bool insert_lsb4_pixel(Pixel * pixel, BinaryMessage *msg);

#endif