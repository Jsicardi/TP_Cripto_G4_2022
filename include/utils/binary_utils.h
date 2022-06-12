#ifndef __BINARY_UTILS_H_
#define __BINARY_UTILS_H_

#include <stdint.h>
#include <stdbool.h>

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
    Sets the msg BinaryMessage structure with the values matching the message starting
    at msg_start and ending at msg_end, so as to be able to perform operations with
    the structure.

    All BinaryMessages should be initialized this way.
 */

bool load_binary_message(uint8_t * msg_start, uint8_t * msg_end, BinaryMessage * msg);

/*
    Sets msg bytes and curr_bit to 0 and curr_ptr and last_ptr point to NULL.
 */

bool unload_binary_message(BinaryMessage * msg, bool free_msg);

/*
    Given a message stored in a BinaryMessage structure. Gets the next bit and stores
    it onto the next_bit variable.

    BinaryMessage structure should be initialized with load_binary_message first.
 */

bool read_next_bit(uint8_t * next_bit, BinaryMessage * msg);

uint8_t bit_identity(uint8_t bit);

uint8_t bit_inversion(uint8_t bit);

#endif