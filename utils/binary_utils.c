#include "../include/utils/binary_utils.h"

#include <stdio.h>

#define BITS_IN_BYTE            8
#define MOST_SIGNIFICANT_BIT_ON 0x80       // 1000 0000
#define ALL_BITS_ON             0xff       // 1111 1111

#define is_bit(__bit__) __bit__ != 0 ? __bit__ != 1 ? false : true : true
#define valid_bit_position(__position__) __position__ < BITS_IN_BYTE && __position__ > 0 ? true : false

bool get_bit_at(uint8_t byte, uint8_t position, uint8_t * bit){

    if(!valid_bit_position(position)) return false;

    /* 
        Moves most significant bit on to the position we wish to read.

        For example, if we wish to read position 2 (3rd bit), it transforms
        1000 0000 to 0010 0000 (shifts 2 bits to the left).

        Afterwards, the result is AND with byte to get just the bit which
        matches the 1 in the result (position 2).

        This results in:
            - 0010 0000 (if the bit in byte was a 1)
            - 0000 0000 (if the bit in byte was a 0)
     */

    byte &= (MOST_SIGNIFICANT_BIT_ON >> position);
    
    /*
        We shift right the bytes so as to have the bit in position at the least
        significant bit position.

        This ensures the resulting integer values are either 1 or 0.
            - 0010 0000 -> 0000 0001 (1)
            - 0000 0000 -> 0000 0000 (0)
    */

    byte >>= (BITS_IN_BYTE - 1) - position;

    // Saves resulting bit
    *(bit) = byte;

    // Remove traces of local variables in memory
    byte     = 0;
    position = 0;
    bit      = NULL;

    return true;
}

bool set_bit_at(uint8_t * byte, uint8_t position, uint8_t bit){

    if(!valid_bit_position(position)) return false;
    if(!is_bit(bit)) return false;

    uint8_t aux = 1;
    aux <<= (BITS_IN_BYTE - 1) - position;

    if(bit == 1){
        
        /*
            If bit is 1, a simple OR with all bits in 0 but the one
            we wish to flip is enough.
         */

        *(byte) = *(byte) | aux;
    } else if(bit == 0){
        
        /*
            If bit is instead 0, we XOR aux with all bits on resulting in the
            inverse pattern.

            For example, 0001 0000 results in 1110 1111

            Afterwards we AND the result with the byte resulting in a flip at
            the bit which is 0 in aux.
        */

        aux ^= ALL_BITS_ON;
        *(byte) = *(byte) & aux;
    }

    // Remove traces of local variables in memory
    byte     = NULL;
    position = 0;
    bit      = 0;

    return true;
}

bool load_binary_message(char * msg_start, char * msg_end, BinaryMessage * msg){
    
    // Messages should end after their beginning
    if(msg_start < msg_end){
        
        // Remove traces of local variables in memory
        msg_start = NULL;
        msg_end   = NULL;
        msg       = NULL;

        return false;
    }

    msg->message        = msg_start;
    msg->curr_byte_ptr  = msg_start;
    msg->last_byte_ptr  = msg_end;
    msg->curr_bit       = 0;

    // Remove traces of local variables in memory
    msg_start = NULL;
    msg_end   = NULL;
    msg       = NULL;
}

bool read_next_bit(uint8_t * next_bit, BinaryMessage * msg){
    
    // Current byte should be before or the last byte
    if(msg->curr_byte_ptr > msg->last_byte_ptr){
        return false;
    }

    /* 
        If we are pointing after the 8th bit (position 7) we must advance 
        to the next byte.
     */

    if(msg->curr_bit >= BITS_IN_BYTE){
        
        // Sets byte pointer to be the next byte
        (msg->curr_byte_ptr)++;
        msg->curr_bit = 0;
        
        /* 
            If the pointer now points outside the message there is nothing 
            more to read.
         */

        if(msg->curr_byte_ptr > msg->last_byte_ptr){
            return false;
        }
    }

    get_bit_at(*(msg->curr_byte_ptr), msg->curr_bit, next_bit);
    msg->curr_bit += 1;

    // Remove traces of local variables in memory
    next_bit = NULL;
    msg      = NULL;

    return true;
}