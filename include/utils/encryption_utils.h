#ifndef __ENCRYPTION_UTILS_H_
#define __ENCRYPTION_UTILS_H_

#include <stdint.h>
#include <stdbool.h>
#include "args.h"

#define DES_BLOCK_SIZE 8
#define AES_128_BLOCK_SIZE 16
#define AES_192_BLOCK_SIZE 24
#define AES_256_BLOCK_SIZE 32

#define MAX_ENCR_LENGTH 5000

/*Given a message, an stegobmp_args structure an encryption buffer and an encrypted_bytes pointer, encrypts the message
with the given parameters and stores it in encryption, and the encrypted bytes in the encrypted_bytes pointer*/
bool encrypt_message(uint8_t * message, size_t message_size, struct stegobmp_args * args,uint8_t ** encryption, uint32_t * encrypted_bytes);

/*Given a encrypted message, an stegobmp_args structure,a message buffer and an decrypted_bytes pointer, decrypts the encrypted message
with the given parameters and stores it in message, and the decrypted bytes in the decrypted_bytes pointer*/
bool decrypt_message(uint8_t * encryption, size_t encryption_size, struct stegobmp_args * args,uint8_t * message,uint32_t * decrypted_bytes);


#endif