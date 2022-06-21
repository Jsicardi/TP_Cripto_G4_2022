#ifndef __BINARY_UTILS_H_
#define __BINARY_UTILS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define BITS_IN_BYTE 8
#define FIST_LOW_BIT_POSITION_LSB1 7
#define FIST_LOW_BIT_POSITION_LSB4 4

typedef struct {
    uint8_t *  message;
    uint8_t *  curr_byte_ptr;
    uint8_t *  last_byte_ptr;
    uint8_t    curr_bit;
} BinaryMessage;

/*
    Given a byte and a position, reads the bit at position from the byte
    and stores it in the bit variable.
 */

bool get_bit_at(uint8_t byte, uint8_t position, uint8_t * bit);

/*
    Given a bit and a position overwrites the bit at specified position
    in the byte variable.
*/

bool set_bit_at(uint8_t * byte, uint8_t position, uint8_t bit);

/*
    Given a byte, swaps its 4 most significant bits with its 4 least significant ones.

    If byte is:                         1011 0101
    
    After returning true,
    byte will be:                       0101 1011
*/

bool swap_low_and_high_bits_in_byte(uint8_t * byte);

/*
    Given an uint32 integer stores in number_bytes an array of 4 bytes containing the number
    in Big Endian.

    number_bytes must be a uint8_t array of at list 4 elements.
 */

bool uint32_to_array_of_uint8(uint8_t * number_bytes, uint32_t number);

/*
    Sets the msg BinaryMessage structure with the values matching the message starting
    at msg_start and ending at msg_end, so as to be able to perform operations with
    the structure.

    All BinaryMessages should be initialized this way.
 */

bool load_binary_message(uint8_t * msg_start, uint8_t * msg_end, BinaryMessage * msg);

/*
    Given a BinaryMessage, sets another BinaryMessage with the same state.
    One can be used for reading and one for writing for example.
 */

bool copy_binary_message(BinaryMessage * from, BinaryMessage * to);

/*
    Sets msg bytes and curr_bit to 0 and curr_ptr and last_ptr point to NULL.
 */

bool unload_binary_message(BinaryMessage * msg, bool free_msg);

/*
    Given a BinaryMessage, sets the message_size_in_bytes variable to be equal to the size in bytes of the message.

    1 byte == 8 bits
 */

bool get_binary_message_size(size_t * message_size_in_bytes, BinaryMessage * msg);

/*
    Given a message stored in a BinaryMessage structure. Gets the next bit and stores
    it onto the next_bit variable.

    BinaryMessage structure should be initialized with load_binary_message first.
 */

bool read_next_bit(uint8_t * next_bit, BinaryMessage * msg);

/* 
    Given a size and a BinaryMessage structure. Makes it so the BinaryMessage structure can
    be used as write buffer of length size bytes.

    After doing the necessary operations. unload_binary_message must be called with free_msg in true.
 */

bool writeable_binary_message(size_t size_in_bytes, BinaryMessage * msg);

/*
    Given a message stored in a BinaryMessage structure and a bit. Sets the next bit.

    BinaryMessage structure should be initialized with writeable_binary_message first.
 */

bool write_next_bit(uint8_t next_bit, BinaryMessage * msg);

/*
    Given a message stored in a BinaryMessage structure. Gets the next byte and stores
    it onto the next_byte variable.

    BinaryMessage structure should be initialized with load_binary_message first.
 */

bool read_next_byte(uint8_t * next_byte, BinaryMessage * msg);

/*
    Given a message stored in a BinaryMessage structure. Gets the next four (4) bytes and stores
    it onto the next_i32 variable.

    BinaryMessage structure should be initialized with load_binary_message first.
 */

bool read_next_i32(uint32_t * next_i32, BinaryMessage * msg);

/*
    Given a message stored in a BinaryMessage structure and an offset of bytes, attempts
    to skip ahead that amount of bytes in the BinaryMessage.

    Could be use in combination with copy_binary_message to go forwards and backwards.

    BinaryMessage structure should be initialized with load_binary_message first.
 */

bool skip_ahead_of_message(size_t bytes, BinaryMessage * msg);

uint8_t bit_identity(uint8_t bit);

uint8_t bit_inversion(uint8_t bit);

#endif