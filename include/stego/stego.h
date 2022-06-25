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

typedef enum { INIT_SNATCH, PATTERN_SNATCH, SIZE_SNATCH, MESSAGE_SNATCH, EXTENSION_SNATCH, FINISHED_SNATCH } lsb_snatcher_state_t;

//TODO add inversion functions (default is identity for all, then change while parsing pattern bytes in lsbi)
typedef struct {
    lsb_snatcher_state_t state;
    uint32_t enc_bytes;
    BinaryMessage w_msg;
    uint8_t (*pattern_functions[4]) (uint8_t bit);
} LsbSnatcherCtx;

/*
    Initializes a LsbSnatcherCtx. Must be called upon before using any snatcher functions.
    A LsbSnatcherCtx is requiered for all snatcher functions.
 */

bool new_lsb_snatcher_ctx(LsbSnatcherCtx * snatcher_ctx);

/*
    Given a byte pointer, a LsbSnatcherCtx and a BinaryMessage, loads the cypher onto the BinaryMessage
    and sets the LsbSnatcherCtx acordingly.

    lsb1_snatcher retrives only last bit of every byte.
 */

bool enc_lsb1_snatcher(uint8_t * byte, void * ctx, BinaryMessage * msg);

/*
    Given a byte pointer, a LsbSnatcherCtx and a BinaryMessage, loads the cypher onto the BinaryMessage
    and sets the LsbSnatcherCtx acordingly.

    lsb4_snatcher retrives last four (4) bits of every byte.
 */

bool enc_lsb4_snatcher(uint8_t * byte, void * ctx, BinaryMessage * msg);

/*
    Given a byte pointer, a LsbSnatcherCtx and a BinaryMessage, loads the cypher onto the BinaryMessage
    and sets the LsbSnatcherCtx acordingly.

    lsb4_snatcher retrives last bit of every byte but uses inversion when needed.
 */

bool enc_lsbi_snatcher(uint8_t * byte, void * ctx, BinaryMessage * msg);

/*
    no_enc work similarly to their enc counterparts but also read
    the file extension after the file.

    file_size will be the LsbContext enc_bytes variables and after those bytes till the
    trailing \0 the extension will be placed.
 */

bool no_enc_lsb1_snatcher(uint8_t * byte, void * ctx, BinaryMessage * msg);

bool no_enc_lsb4_snatcher(uint8_t * byte, void * ctx, BinaryMessage * msg);

bool no_enc_lsbi_snatcher(uint8_t * byte, void *ctx, BinaryMessage *msg);

#endif