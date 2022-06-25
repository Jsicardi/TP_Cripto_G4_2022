#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/utils/args.h"
#include "include/utils/file_utils.h"
#include "include/stego/stego.h"
#include "include/bmp/bmp_file.h"
#include "include/utils/encryption_utils.h"
#include "include/utils/packing_utils.h"

struct stegobmp_args * args;
char* actions[3] = {"-","Embed","Extract"};
char* steg[4] = {"-","LSB1","LSB4","LSBI"};
char* enc[5] = {"None","AES-128","AES-192","AES-256","DES"};
char* modes[5] = {"None","ECB","CFB","OFB","CBC"};



int check_if_compressed(BmpFile * bmp_file, FILE * origin_fd){

    if(!read_bmp_file_metadata(bmp_file, &bmp_file_is_uncompressed, origin_fd)) {

        if(bmp_file_is_compressed(bmp_file)){
            return 1; // File is compressed
        }

        return 2; // Error parsing metadata from origin_fd
    }

    return 0;
}

int embed(struct stegobmp_args * args, BmpFile * bmp_file, FILE * origin_fd){

    /*** Check file is uncompressed ***/
    long savepoint = ftell(origin_fd);

    int ret_val;
    if((ret_val = check_if_compressed(bmp_file, origin_fd)) != 0) return ret_val;

    if(fseek(origin_fd, savepoint, SEEK_SET) != 0) return 2;
    /***********************************/

    /*** Load secret message onto BinaryMessage structure ***/
    BinaryMessage bi_msg;
    
    printf("%s\n", args->in_file);

    if(!pack_message_from_file(args, &bi_msg)) return 8;
    /********************************************************/

    if(!message_can_be_stego(bmp_file, args->steg, &bi_msg)) return 7;

    FILE * destination_fd = fopen(args->out_file, WRITE_BYTES_MODE);

    /*** Copy metadata and offset from input bmp file to output bmp file ***/

    if(!copy_bmp_file_metadata(bmp_file, &bmp_file_is_uncompressed, origin_fd, destination_fd)) {
        
        if(bmp_file_is_compressed(bmp_file)){
            return 1; // File is compressed
        }

        return 2; // Error parsing metadata from origin_fd
    }

    if(!copy_bmp_file_offset(bmp_file, origin_fd, destination_fd)) return 3; // Could not copy BMP File's offset bytes. Could be that metadata is not correct.*/

    /***********************************************************************/

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

    if(!unload_binary_message(&bi_msg, true)) return 9;

    fclose(destination_fd);

    // Reset file_descriptor variable for security measures
    destination_fd = NULL;

    return 0;
}

int extract(struct stegobmp_args * args, BmpFile * bmp_file, FILE * origin_fd) {

    //Check if compressed
    int ret_val;
    if((ret_val = check_if_compressed(bmp_file, origin_fd)) != 0) return ret_val;

    if(!ignore_bmp_file_offset(bmp_file, origin_fd)) return 3; // Could not copy BMP File's offset bytes. Could be that metadata is not correct.*/

    BinaryMessage bi_msg;

    /*** Snatches input bmp file body hidden message into BinaryMessage ***/

    bool (*snatchers[])(uint8_t *, void *, BinaryMessage *) = {
        NULL,
        lsb1_snatcher,
        lsb4_snatcher,
        lsbi_snatcher
    };

    LsbSnatcherCtx ctx;

    new_lsb_snatcher_ctx(&ctx);

    if(args->steg == LSB1 || args->steg == LSB4 || args->steg == LSBI){
        if(!retrive_bmp_file_body(bmp_file, snatchers[args->steg], (void *) &ctx, &bi_msg, origin_fd)) return 4; // Error retrieving body pixels
    }

    /**********************************************************************************************/

    uint8_t * message = malloc(MAX_ENCR_LENGTH);
    uint32_t decrypted_bytes = 0;

    decrypt_message(bi_msg.message, (size_t) ctx.enc_bytes, args, message, &decrypted_bytes);

    if(!unload_binary_message(&bi_msg, true)) return 10;

    if(!load_binary_message(message, message + decrypted_bytes - 1, &bi_msg)) return 10;

    if(!load_to_file(&bi_msg, args->out_file)){
        unload_binary_message(&bi_msg, true);
        return 11;
    }

    if(!unload_binary_message(&bi_msg, true)) return 10;

    return 0;
}

int main(int argc, char * argv[]){

    /*** Load program arguments into stegobmp_args structure ***/
    args = malloc(sizeof(struct stegobmp_args));
    parse_args(argc,argv,args);
    fprintf(stdout,"Parametros:\n"
    "Accion: %s\nArchivo: %s\nPortador: %s\nDestino:%s\nEsteganografia:%s\nEncricpion:%s\nModo:%s\nPassword:%s\n", actions[args->action],args->in_file,args->bmp_file,args->out_file,steg[args->steg],enc[args->enc],modes[args->mode],args->password);
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

    free(args);
    return 0;
}