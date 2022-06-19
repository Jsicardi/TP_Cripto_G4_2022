#ifndef __STEGO_H_
#define __STEGO_H_

#include "../utils/args.h"
#include "../bmp/bmp_file.h"
#include "../utils/binary_utils.h"
#include"../utils/byte_utils.h"

/*
    Given a BmpFile, a lsb_type for hiding and a BinaryMessage. Returns whether it is posible to hide the
    desired message in the given BmpFile.
 */

bool message_can_be_stego(BmpFile * bmp, int lsb_type, BinaryMessage * msg);

bool insert_lsb_pixel(Pixel * pixel, BinaryMessage *msg, int first_low_bit_position);

void get_inversion_functions(BmpBody body,BinaryMessage *msg, FILE * origin_fd,uint8_t (*pattern_functions[]) (uint8_t bit), uint8_t * inversion_byte);

bool invert_lsbi_message_bits(BmpBody body,BinaryMessage *msg, FILE * origin_fd,uint8_t (*pattern_functions[]) (uint8_t bit));

#endif