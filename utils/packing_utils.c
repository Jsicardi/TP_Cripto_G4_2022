#include "../include/utils/packing_utils.h"
#include "../include/utils/file_utils.h"
#include "../include/utils/encryption_utils.h"
#include "../include/utils/byte_utils.h"

bool pack_message_from_file(struct stegobmp_args * args, BinaryMessage * msg, FILE * fd){
    
    if(!load_file(msg, args->in_file)) return false;

    size_t message_size;

    if (!get_binary_message_size(&message_size, msg))                                               return false;

    uint8_t * encrypted_message = NULL, encrypted_bytes_arr[4];
    uint32_t encrypted_bytes;

    if(!encrypt_message(msg->message, message_size, args, &encrypted_message, &encrypted_bytes))    return false;

    if(!uint32_to_array_of_uint8(encrypted_bytes_arr, encrypted_bytes))                             return false;

    if(!prepend_x_bytes(&encrypted_message, message_size, 4, encrypted_bytes_arr))                  return false;

    if(!close_file(msg))                                                                            return false;

    if(!load_binary_message(encrypted_message, encrypted_message + encrypted_bytes, msg))           return false;

    return true;
}