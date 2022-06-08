#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "include/utils/args.h"
#include "include/utils/byte_utils.h"
#include "include/bmp/bmp_file.h"

#define READ_BYTES_MODE "rb"

struct stegobmp_args * args;

int main(int argc, char * argv[]){
    args = malloc(sizeof(struct stegobmp_args));
    printf("Hello\n");
    parse_args(argc,argv,args);
    printf("Action: %d In-file: %s BMP-file: %s out-file:%s Stego:%d Enc:%d Mode:%d Pass:%s\n", args->action,args->in_file,args->bmp_file,args->out_file,args->steg,args->enc,args->mode,args->password);
    
    FILE * file_descriptor = fopen(args->bmp_file, READ_BYTES_MODE);
    BmpFile bmp_file;
    read_bmp_file_metadata(&bmp_file, file_descriptor);

    printf("%d\n", bmp_file.header.bfSize);

    char pixel[3];
    read_x_bytes(pixel, 3, file_descriptor);

    printf("%d %d %d\n", pixel[0], pixel[1], pixel[2]);
    
    free(args);
}
