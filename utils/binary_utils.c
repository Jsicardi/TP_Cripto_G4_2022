#include "../include/utils/binary_utils.h"

#include <stdio.h>
#include <stdlib.h>

#define MOST_SIGNIFICANT_BIT_ON 0x80       // 1000 0000
#define ALL_BITS_ON             0xff       // 1111 1111

#define is_bit(__bit__) (__bit__ != 0 ? __bit__ != 1 ? false : true : true)
#define valid_bit_position(__position__) ((__position__ < BITS_IN_BYTE && __position__ >= 0) ? true : false)

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

bool swap_low_and_high_bits_in_byte(uint8_t * byte){

    /*
        A byte's structure is like the following:

        *byte = high | low;

        Let's say byte has the value 1011 0101 which is 181 in decimal base.

        high | low
        1011 | 0101

        high  = 1011 0000
        low   = 0000 0101

        *byte = high | low = 1011 0101
    */
    
    uint8_t high = *byte & 0xf0; // AND with 1111 0000 to get the four (4) most significant bits
    uint8_t low  = *byte & 0x0f; // AND with 0000 1111 to get the four (4) less significant bits

    /*
        Now we shift right (>>) four (4) bits the most significant bits (high) making them the least significant bits (low).
        high = 1011 0000 >> 0000 1011

        And we shift left (<<) four (4) bits the least significant bits (low) making them the most significant bits (high).
        low  = 0000 0101 >> 0101 0000

        Now high and low have swapped bit placement
        high  = 0000 1011
        low   = 0101 0000

        ORing both we have byte with its high and low reversed
        *byte = high | low = 0101 1011
    */

    high = high >> 4;
    low  = low  << 4;
    
    *byte = low | high;

    return true;
}

bool uint32_to_array_of_uint8(uint8_t * number_bytes, uint32_t number){

    for(int i = 0; i < 4; i++){
        number_bytes[3-i] = number & 0xff;
        number >>= 8;
    }
    
    return true;
}

bool writeable_binary_message(size_t size_in_bytes, BinaryMessage * msg){

    uint8_t * msg_start = malloc(size_in_bytes);

    msg->message       = msg_start;
    msg->curr_byte_ptr = msg_start;
    msg->last_byte_ptr = msg_start + size_in_bytes - 1;
    msg->curr_bit      = 0;

    return true;
}

bool copy_binary_message(BinaryMessage * from, BinaryMessage * to) {

    to->message        = from->message;
    to->curr_byte_ptr  = from->curr_byte_ptr;
    to->last_byte_ptr  = from->last_byte_ptr;
    to->curr_bit       = from->curr_bit;

    return true;
}

bool load_binary_message(uint8_t * msg_start, uint8_t * msg_end, BinaryMessage * msg){
    
    // Messages should end after their beginning
    if(msg_start > msg_end){
        
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

    return true;
}

bool unload_binary_message(BinaryMessage * msg, bool free_msg){

    if(msg == NULL){
        return false;
    };

    msg->curr_byte_ptr = msg->message;
    while(msg->curr_byte_ptr <= msg->last_byte_ptr){
        *(msg->curr_byte_ptr) = 0;
        (msg->curr_byte_ptr)++;
    }


    if(free_msg) free(msg->message);

    msg->message       = NULL;
    msg->curr_byte_ptr = NULL;
    msg->last_byte_ptr = NULL;
    msg->curr_bit      = 0;
    
    return true;
}

bool get_binary_message_size(size_t * message_size_in_bytes, BinaryMessage * msg){
    if(msg->message == NULL || msg->last_byte_ptr == NULL) return false;
    *message_size_in_bytes = msg->last_byte_ptr - msg->message + 1;
    return true;
}

bool read_next_bit(uint8_t * next_bit, BinaryMessage * msg){
    
    // Current byte should be before or the last byte
    if(msg->curr_byte_ptr > msg->last_byte_ptr){
        return false;
    }

    if(!get_bit_at(*(msg->curr_byte_ptr), msg->curr_bit, next_bit)) return false;
    msg->curr_bit += 1;

    /* 
        If we are pointing after the 8th bit (position 7) we must advance 
        to the next byte.
     */

    if(msg->curr_bit >= BITS_IN_BYTE){
        
        // Sets byte pointer to be the next byte
        (msg->curr_byte_ptr)++;
        msg->curr_bit = 0;
    }

    // Remove traces of local variables in memory
    next_bit = NULL;
    msg      = NULL;

    return true;
}

bool write_next_bit(uint8_t next_bit, BinaryMessage * msg){

    // Current byte should be before or the last byte
    if(msg->curr_byte_ptr > msg->last_byte_ptr){
        return false;
    }

    if(!set_bit_at(msg->curr_byte_ptr, msg->curr_bit, next_bit)) return false;
    msg->curr_bit += 1;

    /* 
        If we are pointing after the 8th bit (position 7) we must advance 
        to the next byte.
     */

    if(msg->curr_bit >= BITS_IN_BYTE){
        
        // Sets byte pointer to be the next byte
        (msg->curr_byte_ptr)++;
        msg->curr_bit = 0;
    }

    // Remove traces of local variables in memory
    next_bit = 0;
    msg      = NULL;

    return true;
}

bool read_next_byte(uint8_t * next_byte, BinaryMessage * msg){
    
    // Set next_byte initially as 0.
    *next_byte = 0;
    
    uint8_t bit;

    /*
        Recovers byte by getting bit by bit ORing the last bit
        with the retrived bit and shifting left leaving a 0 for the
        next bit.
     */

    for(uint8_t i = 0; i < BITS_IN_BYTE; i++){
        if(!read_next_bit(&bit, msg)) return false;

        *next_byte <<= 1;
        *next_byte = *next_byte | bit;
    }

    // Reset local pointers for security reasons
    next_byte = NULL;
    msg       = NULL;

    return true;
}

bool read_next_i32(uint32_t * next_i32, BinaryMessage * msg){

    // Set next_i32 initially as 0.
    *next_i32 = 0;

    uint8_t byte;

    /*
        Recovers four (4) bytes (i32) by getting byte by byte ORing the last 8 bits
        with the retrived byte and shifting left a byte leaving eight (8) bits in 0 for the
        next bit.
     */

    for(uint8_t i = 0; i < sizeof(uint32_t); i++){
        if(!read_next_byte(&byte, msg)) return false;

        *next_i32 <<= 8;
        *next_i32 = *next_i32 | byte;
    }

    // Reset local pointers for security reasons
    next_i32  = NULL;
    msg       = NULL;

    return true;
}

bool skip_ahead_of_message(size_t bytes, BinaryMessage * msg){

    if(msg->curr_byte_ptr + bytes > msg->last_byte_ptr){
        return false;
    }
    
    msg->curr_bit = 0;
    msg->curr_byte_ptr += bytes;

    // Reset local variables for security reasons
    bytes = 0;
    msg   = NULL;

    return true;
}

uint8_t bit_identity(uint8_t bit){
    return bit;
}

uint8_t bit_inversion(uint8_t bit){
    return 1-bit;
}
