#include <openssl/evp.h>
#include <string.h>
#include "../include/utils/encryption_utils.h"

const EVP_CIPHER * (* enc_types[4][4]) (void) = {{EVP_aes_128_ecb,EVP_aes_128_cfb,EVP_aes_128_ofb,EVP_aes_128_cbc},{EVP_aes_192_ecb,EVP_aes_192_cfb,EVP_aes_192_ofb,EVP_aes_192_cbc},{EVP_aes_256_ecb,EVP_aes_256_cfb,EVP_aes_256_ofb,EVP_aes_256_cbc},{EVP_des_ecb,EVP_des_cfb,EVP_des_ofb,EVP_des_cbc}};
int block_sizes[4] = {AES_128_BLOCK_SIZE,AES_192_BLOCK_SIZE,AES_256_BLOCK_SIZE,DES_BLOCK_SIZE};

bool encrypt_message(uint8_t * message,struct stegobmp_args * args,uint8_t * encryption, uint32_t * encrypted_bytes){
    EVP_CIPHER_CTX *ctx;
    /*initialize*/
    ctx = EVP_CIPHER_CTX_new();

    /*generate key and iv*/
    unsigned char key[block_sizes[args->enc - 1]];
    unsigned char iv[block_sizes[args->enc - 1]];
    const unsigned char *salt = NULL;
    const EVP_MD *dgst = EVP_sha256();
    EVP_BytesToKey(enc_types[args->enc -1][args->mode - 1](), dgst, salt, args->password, strlen(args->password),1, key, iv);

    /*set parameters for encryption*/
    EVP_EncryptInit_ex(ctx,enc_types[args->enc - 1][args->mode - 1](),NULL, key,iv);
    
    int inl = strlen(message),outl=0,templ=0;
    uint8_t encrypted_message[MAX_ENCR_LENGTH];

    /*encrypts all blocks but the last one*/
    EVP_EncryptUpdate(ctx, encrypted_message, &outl, message, inl);
    
    /*encrypts block + padding*/
    EVP_EncryptFinal_ex(ctx, encrypted_message+outl, &templ);
    
    *encrypted_bytes = (uint32_t) outl + (uint32_t) templ;
    memcpy(encryption, encrypted_message, *encrypted_bytes);

    /*free context*/
    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool decrypt_message(uint8_t * encryption,struct stegobmp_args * args,uint8_t * message,uint32_t * decrypted_bytes){
    EVP_CIPHER_CTX *ctx;
    
    /*initialize*/
    ctx = EVP_CIPHER_CTX_new();

    /*generate key and iv*/
    unsigned char key[block_sizes[args->enc - 1]];
    unsigned char iv[block_sizes[args->enc - 1]];
    const unsigned char *salt = NULL;
    const EVP_MD *dgst = EVP_sha256();
    EVP_BytesToKey(enc_types[args->enc -1][args->mode - 1](), dgst, salt, args->password, strlen(args->password),1, key, iv);


    /*set parameters for decryption*/
    EVP_DecryptInit_ex(ctx,enc_types[args->enc - 1][args->mode - 1](),NULL, key,iv);
    
    int inl = strlen(encryption),outl=0,templ=0;
    uint8_t decrypted_message[MAX_ENCR_LENGTH];

   /*decrypts all blocks but the last one*/
    EVP_DecryptUpdate(ctx, decrypted_message, &outl,encryption,inl);
    
    /*decrypts block + padding*/
    EVP_DecryptFinal_ex(ctx, decrypted_message+outl, &templ);

    *decrypted_bytes = (uint32_t) outl + (uint32_t) templ;
    memcpy(message, decrypted_message, *decrypted_bytes);

    /*free context*/
    EVP_CIPHER_CTX_free(ctx);
    return true;
}