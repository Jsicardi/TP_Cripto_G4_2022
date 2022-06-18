#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/utils/args.h"
#include "include/utils/file_utils.h"
#include "include/stego/stego.h"
#include "include/bmp/bmp_file.h"
#include "include/utils/encryption_utils.h"

struct stegobmp_args * args;

int embed(struct stegobmp_args * args, BmpFile * bmp_file, FILE * origin_fd){

    FILE * destination_fd = fopen(args->out_file, WRITE_BYTES_MODE);

    /*** Copy metadata and offset from input bmp file to output bmp file ***/

    if(!copy_bmp_file_metadata(bmp_file, &bmp_file_is_uncompressed, origin_fd, destination_fd)) {
        
        if(bmp_file_is_compressed(bmp_file)){
            return 1; // File is compressed
        }

        return 2; // Error parsing metadata from origin_fd
    }

    if(!copy_bmp_file_offset(bmp_file, origin_fd, destination_fd)) return 3; // Could not copy BMP File's offset bytes. Could be that metadata is not correct.

    /***********************************************************************/

    /*** Load secret message onto BinaryMessage structure ***/
    BinaryMessage bi_msg;
    
    printf("%s\n", args->in_file);

    if(!load_file(&bi_msg, args->in_file)) return 5;
    /********************************************************/

    /*** Transform input bmp file body to hide BinaryMessage and output it onto the output file ***/

    bool (*bmp_file_body_transformations[])(BmpFile *,bool (*) (Pixel*, BinaryMessage *,int),BinaryMessage *, FILE *, FILE *) = {
        NULL,
        transform_bmp_file_body_lsb1,
        transform_bmp_file_body_lsb4,
        transform_bmp_file_body_lsbi
    };

    if(args->steg == LSB1 || args->steg == LSB4 || args->steg == LSBI){
        if(!(*bmp_file_body_transformations[args->steg]) (bmp_file, &insert_lsb_pixel, &bi_msg, origin_fd, destination_fd)) return 4; // Error copying body pixels
    }

    /**********************************************************************************************/

    if(!close_file(&bi_msg)) return 6;

    fclose(destination_fd);

    // Reset file_descriptor variable for security measures
    destination_fd = NULL;

    return 0;
}

int extract(struct stegobmp_args * args, BmpFile * bmp_file, FILE * origin_fd) {
    return 0;
}

int main(int argc, char * argv[]){

    /*** Load program arguments into stegobmp_args structure ***/
    args = malloc(sizeof(struct stegobmp_args));
    parse_args(argc,argv,args);
    printf("Action: %d In-file: %s BMP-file: %s out-file:%s Stego:%d Enc:%d Mode:%d Pass:%s\n", args->action,args->in_file,args->bmp_file,args->out_file,args->steg,args->enc,args->mode,args->password);
    /***********************************************************/

    // Create filedescriptor for input bmp file
    FILE * origin_fd      = fopen(args->bmp_file, READ_BYTES_MODE);

    BmpFile bmp_file;

    int (*actions[]) (struct stegobmp_args *, BmpFile *, FILE *) = {
        NULL,
        embed,
        extract
    };

    if(args->action == EMBEED || args->action == EXTRACT){
        int ret_val = (*actions[args->action]) (args, &bmp_file, origin_fd);
        if(ret_val != 0) return ret_val;
    }

    fclose(origin_fd);

    // Reset file_descriptor variable for security measures
    origin_fd = NULL;
    
    //ENCRYPT AND DECRYPT TEST. REMOVE LATER
    uint8_t * test_message = "Buenas que onda";
    uint8_t * encryption = malloc(MAX_ENCR_LENGTH);
    uint8_t * decryption = malloc(MAX_ENCR_LENGTH);
    uint32_t encrypted_bytes;
    uint32_t decrypted_bytes;
    encrypt_message(test_message,args,encryption,&encrypted_bytes);
    encryption = (uint8_t *) realloc(encryption, encrypted_bytes+1);
    encryption[encrypted_bytes] = 0;
    printf("Encrypted message: %s\n",encryption);
    decrypt_message(encryption,args,decryption,&decrypted_bytes);
    decryption = (uint8_t *) realloc(decryption, decrypted_bytes+1);
    decryption[decrypted_bytes] = 0;
    printf("Decrypted message: %s\n", decryption);
    free(encryption);
    free(decryption);

    free(args);
    return 0;
}