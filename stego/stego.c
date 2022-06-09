#include "../include/stego/stego.h"

bool insert_lsb4_pixel(Pixel * pixel, BinaryMessage *msg){

    uint8_t next_bit;
    uint8_t* bytes[] = { &(pixel->blue), &(pixel->green), &(pixel->red) };

    for(int i = 0; i < PIXEL_SIZE; i++){

        for(int bit_pos = FIST_LOW_BIT_POSITION; bit_pos < BITS_IN_BYTE; bit_pos++){
            if(read_next_bit(&next_bit, msg)){
                set_bit_at(bytes[i], bit_pos, next_bit);
            }
        }

    }

    return true;
}