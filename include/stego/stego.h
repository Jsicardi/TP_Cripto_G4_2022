#ifndef __STEGO_H_
#define __STEGO_H_

#include "../bmp/bmp_file.h"
#include "../utils/binary_utils.h"
#include"../utils/byte_utils.h"

bool insert_lsb4_pixel(Pixel * pixel, BinaryMessage *msg);


void get_inversion_functions(BmpBody body,BinaryMessage *msg, FILE * origin_fd,uint8_t (*pattern_functions[]) (uint8_t bit));

#endif